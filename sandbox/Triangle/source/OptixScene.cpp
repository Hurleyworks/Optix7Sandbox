
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
	
}

void OptixScene::init(CameraHandle& camera, const json& programGroups)
{
	ScopedStopWatch sw(_FN_);

	renderer = Renderer::create(camera->getScreenWidth(), camera->getScreenHeight());

	pipeHandle = createPipeline(programGroups);
	if (!pipeHandle)
		throw std::runtime_error("Pipeline creation failed");

	buildSBT(camera);
	createAccel();
	engineIsReady = true;
}

void OptixScene::buildSBT(CameraHandle& camera)
{
	CUdeviceptr  raygen_record;
	const size_t raygen_record_size = sizeof(RayGenSbtRecord);
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&raygen_record), raygen_record_size));

	
	updateCamera(camera, rg_sbt);
	
	OPTIX_CHECK(optixSbtRecordPackHeader(config.programs.raygenProgs.front()->get(), &rg_sbt));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(raygen_record),
		&rg_sbt,
		raygen_record_size,
		cudaMemcpyHostToDevice
	));

	CUdeviceptr miss_record;
	size_t      miss_record_size = sizeof(MissSbtRecord);
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&miss_record), miss_record_size));

	Vector4f bg = properties.renderProps->getVal<Vector4f>(RenderKey::BackgroundColor);

	
	ms_sbt.data.r = bg.x();
	ms_sbt.data.b = bg.y();
	ms_sbt.data.g = bg.z();

	OPTIX_CHECK(optixSbtRecordPackHeader(config.programs.missProgs.front()->get(), &ms_sbt));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(miss_record),
		&ms_sbt,
		miss_record_size,
		cudaMemcpyHostToDevice
	));

	CUdeviceptr hitgroup_record;
	size_t      hitgroup_record_size = sizeof(HitGroupSbtRecord);
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&hitgroup_record), hitgroup_record_size));

	
	Vector4f col = properties.renderProps->getVal<Vector4f>(RenderKey::MeshColor);
	hg_sbt.data.r = col.x();
	hg_sbt.data.g = col.y();
	hg_sbt.data.b = col.z();

	OPTIX_CHECK(optixSbtRecordPackHeader(config.programs.hitgroupProgs.front()->get(), &hg_sbt));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(hitgroup_record),
		&hg_sbt,
		hitgroup_record_size,
		cudaMemcpyHostToDevice
	));

	sbt.raygenRecord = raygen_record;
	sbt.missRecordBase = miss_record;
	sbt.missRecordStrideInBytes = sizeof(MissSbtRecord);
	sbt.missRecordCount = 1;
	sbt.hitgroupRecordBase = hitgroup_record;
	sbt.hitgroupRecordStrideInBytes = sizeof(HitGroupSbtRecord);
	sbt.hitgroupRecordCount = 1;

	// safe to delete program groups and modules
	//https://devtalk.nvidia.com/default/topic/1062343/optix/optix-7-module-and-program-group-lifetimes/
	config.programs.hitgroupProgs.clear();
	config.programs.missProgs.clear();
	config.programs.raygenProgs.clear();

	modules.clear();
}


void OptixScene::createAccel()
{
	CUdeviceptr            d_gas_output_buffer;
	{
		

		// Triangle build input
		const std::array<float3, 3> vertices =
		{ {
			  { -0.5f, -0.5f, 0.0f },
			  {  0.5f, -0.5f, 0.0f },
			  {  0.0f,  0.5f, 0.0f }
		} };

		const size_t vertices_size = sizeof(float3) * vertices.size();
		CUdeviceptr d_vertices = 0;
		CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_vertices), vertices_size));
		CUDA_CHECK(cudaMemcpy(
			reinterpret_cast<void*>(d_vertices),
			vertices.data(),
			vertices_size,
			cudaMemcpyHostToDevice
		));

		const uint32_t triangle_input_flags[1] = { OPTIX_GEOMETRY_FLAG_NONE };
		OptixBuildInput triangle_input = {};
		triangle_input.type = OPTIX_BUILD_INPUT_TYPE_TRIANGLES;
		triangle_input.triangleArray.vertexFormat = OPTIX_VERTEX_FORMAT_FLOAT3;
		triangle_input.triangleArray.numVertices = static_cast<uint32_t>(vertices.size());
		triangle_input.triangleArray.vertexBuffers = &d_vertices;
		triangle_input.triangleArray.flags = triangle_input_flags;
		triangle_input.triangleArray.numSbtRecords = 1;

		OptixAccelBufferSizes gas_buffer_sizes;
		OPTIX_CHECK(optixAccelComputeMemoryUsage(context->get(), &config.options.accel_options, &triangle_input,
			1,  // Number of build input
			&gas_buffer_sizes));
		CUdeviceptr d_temp_buffer_gas;
		CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_temp_buffer_gas), gas_buffer_sizes.tempSizeInBytes));

		// non-compacted output
		CUdeviceptr d_buffer_temp_output_gas_and_compacted_size;
		size_t compactedSizeOffset = roundUp<size_t>(gas_buffer_sizes.outputSizeInBytes, 8ull);
		CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(
			&d_buffer_temp_output_gas_and_compacted_size),
			compactedSizeOffset + 8
		));

		OptixAccelEmitDesc emitProperty = {};
		emitProperty.type = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE;
		emitProperty.result = (CUdeviceptr)((char*)d_buffer_temp_output_gas_and_compacted_size + compactedSizeOffset);

		OPTIX_CHECK(optixAccelBuild(
			context->get(),
			0,              // CUDA stream
			&config.options.accel_options,
			&triangle_input,
			1,              // num build inputs
			d_temp_buffer_gas,
			gas_buffer_sizes.tempSizeInBytes,
			d_buffer_temp_output_gas_and_compacted_size,
			gas_buffer_sizes.outputSizeInBytes,
			&gAccel,
			&emitProperty,  // emitted property list
			1               // num emitted properties
		));

		CUDA_CHECK(cudaFree((void*)d_temp_buffer_gas));
		CUDA_CHECK(cudaFree((void*)d_vertices));

		size_t compacted_gas_size;
		CUDA_CHECK(cudaMemcpy(&compacted_gas_size, (void*)emitProperty.result, sizeof(size_t), cudaMemcpyDeviceToHost));

		if (compacted_gas_size < gas_buffer_sizes.outputSizeInBytes)
		{
			CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_gas_output_buffer), compacted_gas_size));

			// use handle as input and output
			OPTIX_CHECK(optixAccelCompact(context->get(), 0, gAccel, d_gas_output_buffer, compacted_gas_size, &gAccel));

			CUDA_CHECK(cudaFree((void*)d_buffer_temp_output_gas_and_compacted_size));
		}
		else
		{
			d_gas_output_buffer = d_buffer_temp_output_gas_and_compacted_size;
		}
	}
}

void OptixScene::syncCamera(CameraHandle& camera)
{

	updateCamera(camera, rg_sbt);

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.raygenRecord),
		&rg_sbt,
		sizeof(RayGenSbtRecord),
		cudaMemcpyHostToDevice
	));
}

void OptixScene::updateCamera(CameraHandle& camera, RayGenSbtRecord& rg_sbt)
{
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

	rg_sbt.data.cam_eye = camEye;
	rg_sbt.data.camera_u = camRight;
	rg_sbt.data.camera_v = camUp;
	rg_sbt.data.camera_w = camForward;
}

void OptixScene::syncBackgoundColor()
{
	Vector4f bg = properties.renderProps->getVal<Vector4f>(RenderKey::BackgroundColor);

	ms_sbt.data.r = bg.x();
	ms_sbt.data.g = bg.y();
	ms_sbt.data.b = bg.z();

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.missRecordBase),
		&ms_sbt,
		sizeof(MissSbtRecord),
		cudaMemcpyHostToDevice
	));
}

void OptixScene::syncMeshColor()
{
	Vector4f col = properties.renderProps->getVal<Vector4f>(RenderKey::MeshColor);

	hg_sbt.data.r = col.x();
	hg_sbt.data.g = col.y();
	hg_sbt.data.b = col.z();

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.hitgroupRecordBase),
		&hg_sbt,
		sizeof(HitGroupSbtRecord),
		cudaMemcpyHostToDevice
	));
}
