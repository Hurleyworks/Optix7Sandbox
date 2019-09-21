
// This source file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

#include "OptixScene.h"
#include "Renderer.h"

using Eigen::Vector4f;
using Eigen::Vector3f;

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
		if (deviceIASoutputBuffer)
			CUDA_CHECK(cudaFree(reinterpret_cast<void*>(deviceIASoutputBuffer)));
	}
	catch (const std::runtime_error& e)
	{
		LOG(CRITICAL) << e.what();
	}
	catch (...)
	{
		LOG(CRITICAL) << "Caught unknow exception";
	}
}

void OptixScene::init(CameraHandle& camera, const json& programGroups)
{
	ScopedStopWatch sw(_FN_);

	renderer = Renderer::create(camera->getScreenWidth(), camera->getScreenHeight());
	renderer->initLaunchParams();

	pipeHandle = createPipeline(programGroups);
	if (!pipeHandle)
		throw std::runtime_error("Pipeline creation failed");

	buildSBT(camera);

	rebuildSceneAccel();

	// Don't release modules and programs since we need them 
	// for dynamic hitgroup record creation
	//modules.clear();
	//config.programs.programs.clear();
}

void OptixScene::addRenderable(RenderableNode& node)
{
	if (!node) return;

	ScopedStopWatch sw(_FN_);

	try
	{
		OptixMeshHandle mesh = OptixMesh::create(node);
		mesh->init(context);
		meshes.push_back(mesh);

		rebuildHitgroupSBT();

		rebuildSceneAccel();
	
		// must restart render
		setRenderRestart(true);
	}
	catch (const std::runtime_error& e)
	{
		LOG(CRITICAL) << e.what();
	}
	catch (const std::bad_alloc& e)
	{
		LOG(CRITICAL) << e.what();
	}
	catch (...)
	{
		LOG(CRITICAL) << "Caught unknow exception";
	}
}

void OptixScene::createRaygenRecord(CameraHandle& camera)
{
	CUdeviceptr  d_raygenRecord;
	
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_raygenRecord), raygenRecordSize));

	updateCamera(camera);
	ProgramGroupHandle raygen = findProgram(raygentName);
	if (!raygen)
		throw std::runtime_error("No raygen program found!");

	OPTIX_CHECK(optixSbtRecordPackHeader(raygen->get(), &raygenRecord));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(d_raygenRecord),
		&raygenRecord,
		raygenRecordSize,
		cudaMemcpyHostToDevice
	));

	sbt.raygenRecord = d_raygenRecord;
}

void OptixScene::createMissRecord()
{
	CUdeviceptr d_missRecord;
	
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_missRecord), missRecordSize));

	// get the miss color from the user
	Vector4f bg = properties.renderProps->getVal<Vector4f>(RenderKey::BackgroundColor);
	missRecord.data.r = bg.x();
	missRecord.data.b = bg.y();
	missRecord.data.g = bg.z();

	ProgramGroupHandle miss = findProgram(radianceMissName);
	if (!miss)
		throw std::runtime_error("No miss program found!");

	OPTIX_CHECK(optixSbtRecordPackHeader(miss->get(), &missRecord));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(d_missRecord),
		&missRecord,
		missRecordSize,
		cudaMemcpyHostToDevice
	));

	sbt.missRecordBase = d_missRecord;
	sbt.missRecordStrideInBytes = missRecordSize;
	sbt.missRecordCount = 1; //  RAY_TYPE_COUNT;
}

void OptixScene::createEmptyHitGroupRecord()
{
	ProgramGroupHandle radianceHit = findProgram(radianceHitName);
	if (!radianceHit)
		throw std::runtime_error("No radiance hit program found!");

	ProgramGroupHandle occlusionHit = findProgram(occlusionHitName);
	if (!occlusionHit)
		throw std::runtime_error("No occlusion hit program found!");

	HitGroupRecord rec = {};
	memset(&rec, 0, hitgroup_record_size);
	OPTIX_CHECK(optixSbtRecordPackHeader(radianceHit->get(), &rec));
	hitgroup_records.push_back(rec);

	OPTIX_CHECK(optixSbtRecordPackHeader(occlusionHit->get(), &rec));
	hitgroup_records.push_back(rec);

	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&sbt.hitgroupRecordBase),
		hitgroup_record_size * hitgroup_records.size()
	));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.hitgroupRecordBase),
		hitgroup_records.data(),
		hitgroup_record_size * hitgroup_records.size(),
		cudaMemcpyHostToDevice
	));

	sbt.hitgroupRecordStrideInBytes = static_cast<unsigned int>(hitgroup_record_size);
	sbt.hitgroupRecordCount = static_cast<unsigned int>(hitgroup_records.size());
}

void OptixScene::rebuildSceneAccel()
{
	const size_t instanceCount = meshes.size();

	// out with the old buffer
	if(deviceIASoutputBuffer)
		CUDA_CHECK(cudaFree(reinterpret_cast<void*>(deviceIASoutputBuffer)));

	std::vector<OptixInstance> optixInstances(instanceCount);

	unsigned int sbtOffset = 0; //  hitgroup_records.size() - RAY_TYPE_COUNT;
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		auto& mesh = meshes[i];
		auto& optixInstance = optixInstances[i];
		memset(&optixInstance, 0, sizeof(OptixInstance));

		optixInstance.flags = OPTIX_INSTANCE_FLAG_NONE;
		optixInstance.instanceId = static_cast<unsigned int>(i);
		optixInstance.sbtOffset = sbtOffset;
		optixInstance.visibilityMask = 1;
		optixInstance.traversableHandle = mesh->getGAS();
		memcpy(optixInstance.transform, mesh->getWorldTransform().data(), sizeof(float) * 12);

		sbtOffset += 1*RAY_TYPE_COUNT;
	}

	const size_t instancesSizeInBytes = sizeof(OptixInstance) * instanceCount;
	CUdeviceptr  deviceInstances;
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&deviceInstances), instancesSizeInBytes));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(deviceInstances),
		optixInstances.data(),
		instancesSizeInBytes,
		cudaMemcpyHostToDevice
	));

	OptixAccelBuildOptions accelOptions = {};
	accelOptions.buildFlags = OPTIX_BUILD_FLAG_NONE;
	accelOptions.operation = OPTIX_BUILD_OPERATION_BUILD;

	OptixBuildInput instanceInput = {};
	instanceInput.type = OPTIX_BUILD_INPUT_TYPE_INSTANCES;
	instanceInput.instanceArray.instances = deviceInstances;
	instanceInput.instanceArray.numInstances = static_cast<unsigned int>(instanceCount);

	OptixAccelBufferSizes IASbufferSizes;
	OPTIX_CHECK(optixAccelComputeMemoryUsage(
		context->get(),
		&accelOptions,
		&instanceInput,
		1, // num build inputs
		&IASbufferSizes
	));

	CUdeviceptr deviceTempBuffer;
	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&deviceTempBuffer),
		IASbufferSizes.tempSizeInBytes
	));
	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&deviceIASoutputBuffer),
		IASbufferSizes.outputSizeInBytes
	));

	OPTIX_CHECK(optixAccelBuild(
		context->get(),
		nullptr,                  // CUDA stream
		&accelOptions,
		&instanceInput,
		1,                  // num build inputs
		deviceTempBuffer,
		IASbufferSizes.tempSizeInBytes,
		deviceIASoutputBuffer,
		IASbufferSizes.outputSizeInBytes,
		&sceneAccel,
		nullptr,            // emitted property list
		0                   // num emitted properties
	));

	CUDA_CHECK(cudaFree(reinterpret_cast<void*>(deviceTempBuffer)));
	CUDA_CHECK(cudaFree(reinterpret_cast<void*>(deviceInstances)));
}

void OptixScene::rebuildHitgroupSBT()
{
	ProgramGroupHandle radianceHit = findProgram(radianceHitName);
	if (!radianceHit)
		throw std::runtime_error("No radiance hit program found!");

	ProgramGroupHandle occulsionHit = findProgram(occlusionHitName);
	if (!occulsionHit)
		throw std::runtime_error("No occulsion hit program found!");

	// delete the old hitgroup records
	CUDA_CHECK(cudaFree(reinterpret_cast<void*>(sbt.hitgroupRecordBase)));
	sbt.hitgroupRecordBase = 0;
	sbt.hitgroupRecordCount = 0;
	sbt.hitgroupRecordStrideInBytes = 0;
	hitgroup_records.clear(); // d'uh forgetting to do this was the cause of much crashing
	
	for (auto mesh : meshes)
	{
		HitGroupRecord rec = {};
		
		OPTIX_CHECK(optixSbtRecordPackHeader(radianceHit->get(), &rec));
		
		rec.data.geometry_data.type = OptixGeometryData::TRIANGLE_MESH;
		rec.data.geometry_data.triangle_mesh.positions = mesh->positions;
		rec.data.geometry_data.triangle_mesh.normals = mesh->normals;
		rec.data.geometry_data.triangle_mesh.texcoords = mesh->texcoords;
		rec.data.geometry_data.triangle_mesh.indices = mesh->indices;

		// use default material 
		rec.data.material_data.pbr = OptixMaterialData::Pbr();

		hitgroup_records.push_back(rec);

		OPTIX_CHECK(optixSbtRecordPackHeader(occulsionHit->get(), &rec));
		hitgroup_records.push_back(rec);
	}

	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&sbt.hitgroupRecordBase),
		hitgroup_record_size * hitgroup_records.size()
	));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.hitgroupRecordBase),
		hitgroup_records.data(),
		hitgroup_record_size * hitgroup_records.size(),
		cudaMemcpyHostToDevice
	));

	sbt.hitgroupRecordStrideInBytes = static_cast<unsigned int>(hitgroup_record_size);
	sbt.hitgroupRecordCount = static_cast<unsigned int>(hitgroup_records.size());
}

void OptixScene::buildSBT(CameraHandle& camera)
{
	createRaygenRecord(camera);
	createMissRecord();

	// need an empty HitGroupRecord for launch
	// we will build a real one as
	// meshes are added
	createEmptyHitGroupRecord();
}

void OptixScene::syncCamera(CameraHandle& camera)
{
	updateCamera(camera);

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.raygenRecord),
		&raygenRecord,
		sizeof(RaygenRecord),
		cudaMemcpyHostToDevice
	));
}

void OptixScene::updateCamera(CameraHandle& camera)
{
	// recalc the view matrix
	if (camera->isDirty())
	{
		camera->getViewMatrix();
	}

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

	raygenRecord.data.cam_eye = camEye;
	raygenRecord.data.camera_u = camRight;
	raygenRecord.data.camera_v = camUp;
	raygenRecord.data.camera_w = camForward;
}

void OptixScene::syncBackgoundColor()
{
	Vector4f bg = properties.renderProps->getVal<Vector4f>(RenderKey::BackgroundColor);

	missRecord.data.r = bg.x();
	missRecord.data.g = bg.y();
	missRecord.data.b = bg.z();

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.missRecordBase),
		&missRecord,
		sizeof(MissRecord),
		cudaMemcpyHostToDevice
	));
}

