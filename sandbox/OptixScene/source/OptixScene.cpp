
// This source file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

#include "OptixScene.h"

using juce::String;
using juce::File;
using juce::StringArray;
using mace::FileServices;
using Eigen::Vector4f;
using Eigen::Vector3f;

namespace
{

	float3 make_float3_from_double(double x, double y, double z)
	{
		return make_float3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	}

	float4 make_float4_from_double(double x, double y, double z, double w)
	{
		return make_float4(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w));
	}

	typedef Record<whitted::HitGroupData> HitGroupRecord;
}

// the bulk of this code is from the Optix7 optixWhitted sample

// ctor
OptixScene::OptixScene (const PropertyService& properties, const OptixConfig& config)
	: OptixEngine(properties, config)
{	
	
}

// dtor
OptixScene::~OptixScene ()
{	
	try
	{
		



		
	}
	catch (std::exception& e)
	{
		LOG(CRITICAL) << e.what();
	}
	catch (...)
	{
		LOG(CRITICAL) << "Caught unknown exception";
	}
}

void OptixScene::init(CameraHandle& camera)
{
	width = camera->getScreenWidth();
	height = camera->getScreenHeight();
	output_buffer.init(CUDAOutputBufferType::ZERO_COPY, width, height);

	m_context = ctx->get();

	addCamera(camera);
	
}


void OptixScene::addBuffer(const uint64_t buf_size, const void* data)
{
	CUdeviceptr buffer = 0;
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&buffer), buf_size));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(buffer),
		data,
		buf_size,
		cudaMemcpyHostToDevice
	));
	m_buffers.push_back(buffer);
}



CUdeviceptr OptixScene::getBuffer(int32_t buffer_index) const
{
	return m_buffers[buffer_index];
}


void OptixScene::finalize()
{
	
	buildMeshAccels();
	buildInstanceAccel();
	createPTXModule();
	createProgramGroups();
	createPipeline();
	createSBT();

	/*m_scene_aabb.invalidate();
	for (const auto mesh : m_meshes)
		m_scene_aabb.include(mesh->world_aabb);

	if (!m_cameras.empty())
		m_cameras.front().setLookat(m_scene_aabb.center());*/

	
	initLaunchParams(camera());
}

void OptixScene::cleanup()
{
}

CameraHandle OptixScene::camera() const
{
	return !m_cameras.empty() ? m_cameras.front() : nullptr;
}

void OptixScene::createContext()
{

	// Initialize CUDA
	CUDA_CHECK(cudaFree(nullptr));

	CUcontext          cuCtx = nullptr;  // zero means take the current context
	OPTIX_CHECK(optixInit());
	OptixDeviceContextOptions options = {};
	options.logCallbackFunction = &contextLogger;
	options.logCallbackLevel = 4;
	OPTIX_CHECK(optixDeviceContextCreate(cuCtx, &options, &m_context));
}

void OptixScene::buildMeshAccels()
{// Problem:
    // The memory requirements of a compacted GAS are unknown prior to building the GAS.
    // Hence, compaction of a GAS requires to build the GAS first and allocating memory for the compacted GAS afterwards.
    // This causes a device-host synchronization point, potentially harming performance.
    // This is most likely the case for small GASes where the actual building and compaction of the GAS is very fast.
    // A naive algorithm processes one GAS at a time with the following steps:
    // 1. compute memory sizes for the build process (temporary buffer size and build buffer size)
    // 2. allocate temporary and build buffer
    // 3. build the GAS (with temporary and build buffer) and compute the compacted size
    // If compacted size is smaller than build buffer size (i.e., compaction is worth it):
    // 4. allocate compacted buffer (final output buffer)
    // 5. compact GAS from build buffer into compact buffer
    //
    // Idea of the algorithm:
    // Batch process the building and compaction of multiple GASes to avoid host-device synchronization.
    // Ideally, the number of synchronization points would be linear with the number of batches rather than the number of GASes.
    // The main constraints for selecting batches of GASes are:
    // a) the peak memory consumption when batch processing GASes, and
    // b) the amount of memory for the output buffer(s), containing the compacted GASes. This is also part of a), but is also important after the build process.
    // For the latter we try to keep it as minimal as possible, i.e., the total memory requirements for the output should equal the sum of the compacted sizes of the GASes.
    // Hence, it should be avoided to waste memory by allocating buffers that are bigger than what is required for a compacted GAS.
    //
    // The peak memory consumption effectively defines the efficiency of the algorithm.
    // If memory was unlimited, compaction isn't needed at all.
    // A lower bound for the peak memory consumption during the build is the output of the process, the size of the compacted GASes.
    // Peak memory consumption effectively defines the memory pool available during the batch building and compaction of GASes.
    //
    // The algorithm estimates the size of the compacted GASes by a give compaction ratio as well as the computed build size of each GAS.
    // The compaction ratio is defined as: size of compacted GAS / size of build output of GAS.
    // The validity of this estimate therefore depends on the assumed compaction ratio.
    // The current algorithm assumes a fixed compaction ratio.
    // Other strategies could be:
    // - update the compaction ration on the fly by do statistics on the already processed GASes to have a better guess for the remaining batches
    // - multiple compaction rations by type of GAS (e.g., motion vs static), since the type of GAS impacts the compaction ratio
    // Further, compaction may be skipped for GASes that do not benefit from compaction (compaction ratio of 1.0).
    //
    // Before selecting GASes for a batch, all GASes are sorted by size (their build size).
    // Big GASes are handled before smaller GASes as this will increase the likelihood of the peak memory consumption staying close to the minimal memory consumption.
    // This also increase the benefit of batching since small GASes that benefit most from avoiding synchronizations are built "together".
    // The minimum batch size is one GAS to ensure forward process.
    //
    // Goal:
    // Estimate the required output size (the minimal peak memory consumption) and work within these bounds.
    // Batch process GASes as long as they are expected to fit into the memory bounds (non strict).
    //
    // Assumptions:
    // The inputs to each GAS are already in device memory and are needed afterwards.
    // Otherwise this could be factored into the peak memory consumption.
    // E.g., by uploading the input data to the device only just before building the GAS and releasing it right afterwards.
    //
    // Further, the peak memory consumption of the application / system is influenced by many factors unknown to this algorithm.
    // E.g., if it is known that a big pool of memory is needed after GAS building anyways (e.g., textures that need to be present on the device),
    // peak memory consumption will be higher eventually and the GAS build process could already make use of a bigger memory pool.
    //
    // TODOs:
    // - compaction ratio estimation / updating
    // - handling of non-compactable GASes
    // - integration of GAS input data upload / freeing
    // - add optional hard limits / check for hard memory limits (shrink batch size / abort, ...)
    //////////////////////////////////////////////////////////////////////////

    // Magic constants:

    // see explanation above
	constexpr double initialCompactionRatio = 0.5;

	// It is assumed that trace is called later when the GASes are still in memory.
	// We know that the memory consumption at that time will at least be the compacted GASes + some CUDA stack space.
	// Add a "random" 250MB that we can use here, roughly matching CUDA stack space requirements.
	constexpr size_t additionalAvailableMemory = 250 * 1024 * 1024;

	//////////////////////////////////////////////////////////////////////////

	OptixAccelBuildOptions accel_options = {};
	accel_options.buildFlags = OPTIX_BUILD_FLAG_ALLOW_COMPACTION;
	accel_options.operation = OPTIX_BUILD_OPERATION_BUILD;

	struct GASInfo {
		std::vector<OptixBuildInput> buildInputs;
		OptixAccelBufferSizes gas_buffer_sizes;
		std::shared_ptr<MeshGroup> mesh;
	};
	std::multimap<size_t, GASInfo> gases;
	size_t totalTempOutputSize = 0;
	/*const*/ uint32_t triangle_input_flags = OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT;

	for (size_t i = 0; i < m_meshes.size(); ++i)
	{
		auto& mesh = m_meshes[i];

		const size_t num_subMeshes = mesh->indices.size();
		std::vector<OptixBuildInput> buildInputs(num_subMeshes);

		assert(mesh->positions.size() == num_subMeshes &&
			mesh->normals.size() == num_subMeshes &&
			mesh->texcoords.size() == num_subMeshes);

		for (size_t i = 0; i < num_subMeshes; ++i)
		{
			OptixBuildInput& triangle_input = buildInputs[i];
			memset(&triangle_input, 0, sizeof(OptixBuildInput));
			triangle_input.type = OPTIX_BUILD_INPUT_TYPE_TRIANGLES;
			triangle_input.triangleArray.vertexFormat = OPTIX_VERTEX_FORMAT_FLOAT3;
			triangle_input.triangleArray.vertexStrideInBytes =
				mesh->positions[i].byte_stride ?
				mesh->positions[i].byte_stride :
				sizeof(float3),
				triangle_input.triangleArray.numVertices = mesh->positions[i].count;
			triangle_input.triangleArray.vertexBuffers = &(mesh->positions[i].data);
			triangle_input.triangleArray.indexFormat =
				mesh->indices[i].elmt_byte_size == 2 ?
				OPTIX_INDICES_FORMAT_UNSIGNED_SHORT3 :
				OPTIX_INDICES_FORMAT_UNSIGNED_INT3;
			triangle_input.triangleArray.indexStrideInBytes =
				mesh->indices[i].byte_stride ?
				mesh->indices[i].byte_stride :
				mesh->indices[i].elmt_byte_size * 3;
			triangle_input.triangleArray.numIndexTriplets = mesh->indices[i].count / 3;
			triangle_input.triangleArray.indexBuffer = mesh->indices[i].data;
			triangle_input.triangleArray.flags = &triangle_input_flags;
			triangle_input.triangleArray.numSbtRecords = 1;
		}

		OptixAccelBufferSizes gas_buffer_sizes;
		OPTIX_CHECK(optixAccelComputeMemoryUsage(m_context, &accel_options, buildInputs.data(),
			static_cast<unsigned int>(num_subMeshes), &gas_buffer_sizes));

		totalTempOutputSize += gas_buffer_sizes.outputSizeInBytes;
		GASInfo g = { std::move(buildInputs), gas_buffer_sizes, mesh };
		gases.emplace(gas_buffer_sizes.outputSizeInBytes, g);
	}

	size_t totalTempOutputProcessedSize = 0;
	size_t usedCompactedOutputSize = 0;
	double compactionRatio = initialCompactionRatio;

	CuBuffer<char> d_temp;
	CuBuffer<char> d_temp_output;
	CuBuffer<size_t> d_temp_compactedSizes;

	OptixAccelEmitDesc emitProperty = {};
	emitProperty.type = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE;

	while (!gases.empty())
	{
		// The estimated total output size that we end up with when using compaction.
		// It defines the minimum peak memory consumption, but is unknown before actually building all GASes.
		// Working only within these memory constraints results in an actual peak memory consumption that is very close to the minimal peak memory consumption.
		size_t remainingEstimatedTotalOutputSize =
			(size_t)((totalTempOutputSize - totalTempOutputProcessedSize) * compactionRatio);
		size_t availableMemPoolSize = remainingEstimatedTotalOutputSize + additionalAvailableMemory;
		// We need to fit the following things into availableMemPoolSize:
		// - temporary buffer for building a GAS (only during build, can be cleared before compaction)
		// - build output buffer of a GAS
		// - size (actual number) of a compacted GAS as output of a build
		// - compacted GAS

		size_t batchNGASes = 0;
		size_t batchBuildOutputRequirement = 0;
		size_t batchBuildMaxTempRequirement = 0;
		size_t batchBuildCompactedRequirement = 0;
		for (auto it = gases.rbegin(); it != gases.rend(); it++)
		{
			batchBuildOutputRequirement += it->second.gas_buffer_sizes.outputSizeInBytes;
			batchBuildCompactedRequirement += (size_t)(it->second.gas_buffer_sizes.outputSizeInBytes * compactionRatio);
			// roughly account for the storage of the compacted size, although that goes into a separate buffer
			batchBuildOutputRequirement += 8ull;
			// make sure that all further output pointers are 256 byte aligned
			batchBuildOutputRequirement = roundUp<size_t>(batchBuildOutputRequirement, 256ull);
			// temp buffer is shared for all builds in the batch
			batchBuildMaxTempRequirement = std::max(batchBuildMaxTempRequirement, it->second.gas_buffer_sizes.tempSizeInBytes);
			batchNGASes++;
			if ((batchBuildOutputRequirement + batchBuildMaxTempRequirement + batchBuildCompactedRequirement) > availableMemPoolSize)
				break;
		}

		// d_temp may still be available from a previous batch, but is freed later if it is "too big"
		d_temp.allocIfRequired(batchBuildMaxTempRequirement);

		// trash existing buffer if it is more than 10% bigger than what we need
		// if it is roughly the same, we keep it
		if (d_temp_output.byteSize() > batchBuildOutputRequirement * 1.1)
			d_temp_output.free();
		d_temp_output.allocIfRequired(batchBuildOutputRequirement);

		// this buffer is assumed to be very small
		// trash d_temp_compactedSizes if it is at least 20MB in size and at least double the size than required for the next run
		if (d_temp_compactedSizes.reservedCount() > batchNGASes * 2 && d_temp_compactedSizes.byteSize() > 20 * 1024 * 1024)
			d_temp_compactedSizes.free();
		d_temp_compactedSizes.allocIfRequired(batchNGASes);

		// sum of build output size of GASes, excluding alignment
		size_t batchTempOutputSize = 0;
		// sum of size of compacted GASes
		size_t batchCompactedSize = 0;

		auto it = gases.rbegin();
		for (size_t i = 0, tempOutputAlignmentOffset = 0; i < batchNGASes; ++i)
		{
			emitProperty.result = d_temp_compactedSizes.get(i);
			GASInfo& info = it->second;

			OPTIX_CHECK(optixAccelBuild(m_context, 0,   // CUDA stream
				&accel_options,
				info.buildInputs.data(),
				static_cast<unsigned int>(info.buildInputs.size()),
				d_temp.get(),
				d_temp.byteSize(),
				d_temp_output.get(tempOutputAlignmentOffset),
				info.gas_buffer_sizes.outputSizeInBytes,
				&info.mesh->gas_handle,
				&emitProperty,  // emitted property list
				1               // num emitted properties
			));

			tempOutputAlignmentOffset += roundUp<size_t>(info.gas_buffer_sizes.outputSizeInBytes, 256ull);
			it++;
		}

		// trash d_temp if it is at least 20MB in size
		if (d_temp.byteSize() > 20 * 1024 * 1024)
			d_temp.free();

		// download all compacted sizes to allocate final output buffers for these GASes
		std::vector<size_t> h_compactedSizes(batchNGASes);
		d_temp_compactedSizes.download(h_compactedSizes.data());

		//////////////////////////////////////////////////////////////////////////
		// TODO:
		// Now we know the actual memory requirement of the compacted GASes.
		// Based on that we could shrink the batch if the compaction ratio is bad and we need to strictly fit into the/any available memory pool.
		bool canCompact = false;
		it = gases.rbegin();
		for (size_t i = 0; i < batchNGASes; ++i)
		{
			GASInfo& info = it->second;
			if (info.gas_buffer_sizes.outputSizeInBytes > h_compactedSizes[i])
			{
				canCompact = true;
				break;
			}
			it++;
		}

		if (canCompact)
		{
			//////////////////////////////////////////////////////////////////////////
			// "batch allocate" the compacted buffers
			it = gases.rbegin();
			for (size_t i = 0; i < batchNGASes; ++i)
			{
				GASInfo& info = it->second;
				batchCompactedSize += h_compactedSizes[i];
				CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&info.mesh->d_gas_output), h_compactedSizes[i]));
				totalTempOutputProcessedSize += info.gas_buffer_sizes.outputSizeInBytes;
				it++;
			}

			it = gases.rbegin();
			for (size_t i = 0; i < batchNGASes; ++i)
			{
				GASInfo& info = it->second;
				OPTIX_CHECK(optixAccelCompact(m_context, 0, info.mesh->gas_handle, info.mesh->d_gas_output,
					h_compactedSizes[i], &info.mesh->gas_handle));
				it++;
			}
		}
		else
		{
			it = gases.rbegin();
			for (size_t i = 0, tempOutputAlignmentOffset = 0; i < batchNGASes; ++i)
			{
				GASInfo& info = it->second;
				info.mesh->d_gas_output = d_temp_output.get(tempOutputAlignmentOffset);
				batchCompactedSize += h_compactedSizes[i];
				totalTempOutputProcessedSize += info.gas_buffer_sizes.outputSizeInBytes;

				tempOutputAlignmentOffset += roundUp<size_t>(info.gas_buffer_sizes.outputSizeInBytes, 256ull);
				it++;
			}
			d_temp_output.release();
		}

		usedCompactedOutputSize += batchCompactedSize;

		gases.erase(it.base(), gases.end());
	}
}

void OptixScene::buildInstanceAccel(int rayTypeCount)
{
	const size_t num_instances = m_meshes.size();

	std::vector<OptixInstance> optix_instances(num_instances);

	unsigned int sbt_offset = 0;
	for (size_t i = 0; i < m_meshes.size(); ++i)
	{
		auto  mesh = m_meshes[i];
		auto& optix_instance = optix_instances[i];
		memset(&optix_instance, 0, sizeof(OptixInstance));

		optix_instance.flags = OPTIX_INSTANCE_FLAG_NONE;
		optix_instance.instanceId = static_cast<unsigned int>(i);
		optix_instance.sbtOffset = sbt_offset;
		optix_instance.visibilityMask = 1;
		optix_instance.traversableHandle = mesh->gas_handle;
		memcpy(optix_instance.transform, mesh->transform.data(), sizeof(float) * 12);

		sbt_offset += static_cast<unsigned int>(mesh->indices.size()) * rayTypeCount;  // one sbt record per GAS build input per RAY_TYPE
	}

	const size_t instances_size_in_bytes = sizeof(OptixInstance) * num_instances;
	CUdeviceptr  d_instances;
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_instances), instances_size_in_bytes));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(d_instances),
		optix_instances.data(),
		instances_size_in_bytes,
		cudaMemcpyHostToDevice
	));

	OptixBuildInput instance_input = {};
	instance_input.type = OPTIX_BUILD_INPUT_TYPE_INSTANCES;
	instance_input.instanceArray.instances = d_instances;
	instance_input.instanceArray.numInstances = static_cast<unsigned int>(num_instances);

	OptixAccelBuildOptions accel_options = {};
	accel_options.buildFlags = OPTIX_BUILD_FLAG_NONE;
	accel_options.operation = OPTIX_BUILD_OPERATION_BUILD;

	OptixAccelBufferSizes ias_buffer_sizes;
	OPTIX_CHECK(optixAccelComputeMemoryUsage(
		m_context,
		&accel_options,
		&instance_input,
		1, // num build inputs
		&ias_buffer_sizes
	));

	CUdeviceptr d_temp_buffer;
	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&d_temp_buffer),
		ias_buffer_sizes.tempSizeInBytes
	));
	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&m_d_ias_output_buffer),
		ias_buffer_sizes.outputSizeInBytes
	));

	OPTIX_CHECK(optixAccelBuild(
		m_context,
		nullptr,                  // CUDA stream
		&accel_options,
		&instance_input,
		1,                  // num build inputs
		d_temp_buffer,
		ias_buffer_sizes.tempSizeInBytes,
		m_d_ias_output_buffer,
		ias_buffer_sizes.outputSizeInBytes,
		&m_ias_handle,
		nullptr,            // emitted property list
		0                   // num emitted properties
	));

	CUDA_CHECK(cudaFree(reinterpret_cast<void*>(d_temp_buffer)));
	CUDA_CHECK(cudaFree(reinterpret_cast<void*>(d_instances)));
}

void OptixScene::createPTXModule()
{
	OptixModuleCompileOptions module_compile_options = {};
	module_compile_options.optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
	module_compile_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_LINEINFO;

	m_pipeline_compile_options = {};
	m_pipeline_compile_options.usesMotionBlur = false;
	m_pipeline_compile_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING;
	m_pipeline_compile_options.numPayloadValues = whitted::NUM_PAYLOAD_VALUES;
	m_pipeline_compile_options.numAttributeValues = 2; // TODO
	m_pipeline_compile_options.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE; // should be OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW;
	m_pipeline_compile_options.pipelineLaunchParamsVariableName = "params";

	const std::string ptx = getPtxString("whitted.ptx");

	m_ptx_module = {};
	char log[2048];
	size_t sizeof_log = sizeof(log);
	OPTIX_CHECK_LOG(optixModuleCreateFromPTX(
		m_context,
		&module_compile_options,
		&m_pipeline_compile_options,
		ptx.c_str(),
		ptx.size(),
		log,
		&sizeof_log,
		&m_ptx_module
	));
}

std::string OptixScene::getPtxString(const std::string& name)
{
	std::string resourceFolder = properties.renderProps->getVal<std::string>(RenderKey::ResourceFolder);
	File f(resourceFolder);
	if (!f.exists())
		throw std::runtime_error("No resource folder found at " + resourceFolder);

	String ptxFolder = resourceFolder + "/ptx";
	StringArray ptxFiles;
	String wildCard("*.ptx");
	FileServices::getFiles(ptxFolder, ptxFiles, wildCard);

	String ptxStr = String::empty;

	for (auto ptx : ptxFiles)
	{
		File f(ptx);
		if (!f.existsAsFile()) continue;

		if (f.getFileName() == String(name))
		{
			LOG(DBUG) << "Found " << f.getFileName();
			ptxStr = f.loadFileAsString();
			break;
		}
	}

	if (ptxStr == String::empty)
		throw std::runtime_error("Could not load ptx file: " + name);

	return ptxStr.toStdString();
}
void OptixScene::createProgramGroups()
{
	OptixProgramGroupOptions program_group_options = {};

	char log[2048];
	size_t sizeof_log = sizeof(log);

	//
	// Ray generation
	//
	{

		OptixProgramGroupDesc raygen_prog_group_desc = {};
		raygen_prog_group_desc.kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
		raygen_prog_group_desc.raygen.module = m_ptx_module;
		raygen_prog_group_desc.raygen.entryFunctionName = "__raygen__pinhole";

		OPTIX_CHECK_LOG(optixProgramGroupCreate(
			m_context,
			&raygen_prog_group_desc,
			1,                             // num program groups
			&program_group_options,
			log,
			&sizeof_log,
			&m_raygen_prog_group
		)
		);
	}

	//
	// Miss
	//
	{
		OptixProgramGroupDesc miss_prog_group_desc = {};
		miss_prog_group_desc.kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
		miss_prog_group_desc.miss.module = m_ptx_module;
		miss_prog_group_desc.miss.entryFunctionName = "__miss__constant_radiance";
		sizeof_log = sizeof(log);
		OPTIX_CHECK_LOG(optixProgramGroupCreate(
			m_context,
			&miss_prog_group_desc,
			1,                             // num program groups
			&program_group_options,
			log,
			&sizeof_log,
			&m_radiance_miss_group
		)
		);

		memset(&miss_prog_group_desc, 0, sizeof(OptixProgramGroupDesc));
		miss_prog_group_desc.kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
		miss_prog_group_desc.miss.module = nullptr;  // NULL miss program for occlusion rays
		miss_prog_group_desc.miss.entryFunctionName = nullptr;
		sizeof_log = sizeof(log);
		OPTIX_CHECK_LOG(optixProgramGroupCreate(
			m_context,
			&miss_prog_group_desc,
			1,                             // num program groups
			&program_group_options,
			log,
			&sizeof_log,
			&m_occlusion_miss_group
		)
		);
	}

	//
	// Hit group
	//
	{
		OptixProgramGroupDesc hit_prog_group_desc = {};
		hit_prog_group_desc.kind = OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
		hit_prog_group_desc.hitgroup.moduleCH = m_ptx_module;
		hit_prog_group_desc.hitgroup.entryFunctionNameCH = "__closesthit__radiance";
		sizeof_log = sizeof(log);
		OPTIX_CHECK_LOG(optixProgramGroupCreate(
			m_context,
			&hit_prog_group_desc,
			1,                             // num program groups
			&program_group_options,
			log,
			&sizeof_log,
			&m_radiance_hit_group
		)
		);

		memset(&hit_prog_group_desc, 0, sizeof(OptixProgramGroupDesc));
		hit_prog_group_desc.kind = OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
		hit_prog_group_desc.hitgroup.moduleCH = m_ptx_module;
		hit_prog_group_desc.hitgroup.entryFunctionNameCH = "__closesthit__occlusion";
		sizeof_log = sizeof(log);
		OPTIX_CHECK(optixProgramGroupCreate(
			m_context,
			&hit_prog_group_desc,
			1,                             // num program groups
			&program_group_options,
			log,
			&sizeof_log,
			&m_occlusion_hit_group
		)
		);
	}
}

void OptixScene::createPipeline()
{
	OptixProgramGroup program_groups[] =
	{
		m_raygen_prog_group,
		m_radiance_miss_group,
		m_occlusion_miss_group,
		m_radiance_hit_group,
		m_occlusion_hit_group
	};

	OptixPipelineLinkOptions pipeline_link_options = {};
	pipeline_link_options.maxTraceDepth = 2;
	pipeline_link_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_FULL;
	pipeline_link_options.overrideUsesMotionBlur = false;

	char log[2048];
	size_t sizeof_log = sizeof(log);
	OPTIX_CHECK_LOG(optixPipelineCreate(
		m_context,
		&m_pipeline_compile_options,
		&pipeline_link_options,
		program_groups,
		sizeof(program_groups) / sizeof(program_groups[0]),
		log,
		&sizeof_log,
		&m_pipeline
	));
}

void OptixScene::createSBT()
{
	{
		const size_t raygen_record_size = sizeof(EmptyRecord);
		CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&m_sbt.raygenRecord), raygen_record_size));

		EmptyRecord rg_sbt;
		OPTIX_CHECK(optixSbtRecordPackHeader(m_raygen_prog_group, &rg_sbt));
		CUDA_CHECK(cudaMemcpy(
			reinterpret_cast<void*>(m_sbt.raygenRecord),
			&rg_sbt,
			raygen_record_size,
			cudaMemcpyHostToDevice
		));
	}

	{
		const size_t miss_record_size = sizeof(EmptyRecord);
		CUDA_CHECK(cudaMalloc(
			reinterpret_cast<void**>(&m_sbt.missRecordBase),
			miss_record_size * whitted::RAY_TYPE_COUNT
		));

		EmptyRecord ms_sbt[whitted::RAY_TYPE_COUNT];
		OPTIX_CHECK(optixSbtRecordPackHeader(m_radiance_miss_group, &ms_sbt[0]));
		OPTIX_CHECK(optixSbtRecordPackHeader(m_occlusion_miss_group, &ms_sbt[1]));

		CUDA_CHECK(cudaMemcpy(
			reinterpret_cast<void*>(m_sbt.missRecordBase),
			ms_sbt,
			miss_record_size * whitted::RAY_TYPE_COUNT,
			cudaMemcpyHostToDevice
		));
		m_sbt.missRecordStrideInBytes = static_cast<uint32_t>(miss_record_size);
		m_sbt.missRecordCount = whitted::RAY_TYPE_COUNT;
	}

	{
		std::vector<HitGroupRecord> hitgroup_records;
		for (const auto mesh : m_meshes)
		{
			for (size_t i = 0; i < mesh->material_idx.size(); ++i)
			{
				HitGroupRecord rec = {};
				OPTIX_CHECK(optixSbtRecordPackHeader(m_radiance_hit_group, &rec));
				rec.data.geometry_data.type = GeometryData::TRIANGLE_MESH;
				rec.data.geometry_data.triangle_mesh.positions = mesh->positions[i];
				rec.data.geometry_data.triangle_mesh.normals = mesh->normals[i];
				rec.data.geometry_data.triangle_mesh.texcoords = mesh->texcoords[i];
				rec.data.geometry_data.triangle_mesh.indices = mesh->indices[i];

				const int32_t mat_idx = mesh->material_idx[i];
				if (mat_idx >= 0)
					rec.data.material_data.pbr = m_materials[mat_idx];
				else
					rec.data.material_data.pbr = MaterialData::Pbr();
				hitgroup_records.push_back(rec);

				OPTIX_CHECK(optixSbtRecordPackHeader(m_occlusion_hit_group, &rec));
				hitgroup_records.push_back(rec);
			}
		}

		const size_t hitgroup_record_size = sizeof(HitGroupRecord);
		CUDA_CHECK(cudaMalloc(
			reinterpret_cast<void**>(&m_sbt.hitgroupRecordBase),
			hitgroup_record_size * hitgroup_records.size()
		));
		CUDA_CHECK(cudaMemcpy(
			reinterpret_cast<void*>(m_sbt.hitgroupRecordBase),
			hitgroup_records.data(),
			hitgroup_record_size * hitgroup_records.size(),
			cudaMemcpyHostToDevice
		));

		m_sbt.hitgroupRecordStrideInBytes = static_cast<unsigned int>(hitgroup_record_size);
		m_sbt.hitgroupRecordCount = static_cast<unsigned int>(hitgroup_records.size());
	}
}

void OptixScene::initLaunchParams(CameraHandle& camera)
{
	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&params.accum_buffer),
		width * height * sizeof(float4)
	));
	params.frame_buffer = nullptr; // Will be set when output buffer is mapped

	params.subframe_index = 0u;
	

	const float loffset = m_scene_aabb.max().maxCoeff();

	Vector3f c = m_scene_aabb.center();
	float3 center = make_float3(c.x(), c.y(), c.z());

	// TODO: add light support to sutil::Scene
	std::vector<Light::Point> lights(2);
	lights[0].color = { 1.0f, 1.0f, 0.8f };
	lights[0].intensity = 5.0f;
	lights[0].position = center + make_float3(loffset);
	lights[0].falloff = Light::Falloff::QUADRATIC;
	lights[1].color = { 0.8f, 0.8f, 1.0f };
	lights[1].intensity = 3.0f;
	lights[1].position = center + make_float3(-loffset, 0.5f * loffset, -0.5f * loffset);
	lights[1].falloff = Light::Falloff::QUADRATIC;

	params.lights.count = static_cast<uint32_t>(lights.size());
	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&params.lights.data),
		lights.size() * sizeof(Light::Point)
	));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(params.lights.data),
		lights.data(),
		lights.size() * sizeof(Light::Point),
		cudaMemcpyHostToDevice
	));

	params.miss_color = make_float3(0.1f);


	// recalc the view matrix
	camera->getViewMatrix();

	const Vector3f& eye = camera->getEyePoint();
	const Vector3f& forward = camera->getFoward();
	const Vector3f& right = camera->getRight();
	const Vector3f& up = camera->getUp();

	float ulen, vlen, wlen;
	wlen = forward.norm();
	vlen = wlen * tanf(0.5f * camera->getFOV() * M_PIf / 180.0f);
	ulen = vlen * camera->getAspect();;

	float3 camRight, camUp, camForward, camEye;
	camUp = make_float3(up.x(), up.y(), up.z());
	camUp *= vlen;

	camRight = make_float3(right.x(), right.y(), right.z());
	camRight *= ulen;

	camForward = make_float3(forward.x(), forward.y(), forward.z());
	camEye = make_float3(eye.x(), eye.y(), eye.z());

	params.eye = camEye;
	params.U = camRight;
	params.V = camUp;
	params.W = camForward;

	//CUDA_CHECK( cudaStreamCreate( &stream ) );
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_params), sizeof(whitted::LaunchParams)));

	params.handle = traversableHandle();
}

void OptixScene::launchSubframe(CameraHandle& camera)
{
	// Launch
	uchar4 * result_buffer_data = output_buffer.map();
	params.frame_buffer = result_buffer_data;
	CUDA_CHECK(cudaMemcpyAsync(reinterpret_cast<void*>(d_params),
		&params,
		sizeof(whitted::LaunchParams),
		cudaMemcpyHostToDevice,
		0 // stream
	));

	OPTIX_CHECK(optixLaunch(
		pipeline(),
		0,             // stream
		reinterpret_cast<CUdeviceptr>(d_params),
		sizeof(whitted::LaunchParams),
		sbt(),
		width,  // launch width
		height, // launch height
		1       // launch depth
	));
	output_buffer.unmap();
	CUDA_SYNC_CHECK();

	// grab the render from Optix
	sabi::PixelBuffer& buffer = camera->getPixelBuffer();
	std::memcpy(buffer.uint8Pixels.data(), output_buffer.getHostPointer(), buffer.byteCountUint8());

	// update the accumulator
	//++state.params.subframe_index;
}


