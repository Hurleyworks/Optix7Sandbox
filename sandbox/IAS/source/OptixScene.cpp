
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

	pipeHandle = createPipeline(programGroups);
	if (!pipeHandle)
		throw std::runtime_error("Pipeline creation failed");

	buildSBT(camera);

	rebuildSceneAccel();

	// ok to release modules and program groups
	modules.clear();
	config.programs.programs.clear();
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

		rebuildSceneAccel();
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

void OptixScene::rebuildSceneAccel()
{
	const size_t instanceCount = meshes.size();

	// out with the old buffer
	if(deviceIASoutputBuffer)
		CUDA_CHECK(cudaFree(reinterpret_cast<void*>(deviceIASoutputBuffer)));

	std::vector<OptixInstance> optixInstances(instanceCount);

	unsigned int sbtOffset = 0;
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

		// THIS CAUSES A CRASH AFTER INSTANCE COUNT REACHES 12
		//sbtOffset += static_cast<unsigned int>(mesh->indicesSize());  // one sbt record per GAS build input per RAY_TYPE
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

void OptixScene::buildSBT(CameraHandle& camera)
{
	CUdeviceptr  raygenRecord;
	const size_t raygenRecordSize = sizeof(RayGenSbtRecord);
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&raygenRecord), raygenRecordSize));

	updateCamera(camera);
	ProgramGroupHandle raygen = findProgram("camera_group");

	OPTIX_CHECK(optixSbtRecordPackHeader(raygen->get(), &raygenSBT));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(raygenRecord),
		&raygenSBT,
		raygenRecordSize,
		cudaMemcpyHostToDevice
	));

	CUdeviceptr missRecord;
	size_t      missRecordSize = sizeof(MissSbtRecord);
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&missRecord), missRecordSize));

	// get the miss color from the user
	Vector4f bg = properties.renderProps->getVal<Vector4f>(RenderKey::BackgroundColor);
	missSBT.data.r = bg.x();
	missSBT.data.b = bg.y();
	missSBT.data.g = bg.z();

	ProgramGroupHandle miss = findProgram("miss_group");
	OPTIX_CHECK(optixSbtRecordPackHeader(miss->get(), &missSBT));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(missRecord),
		&missSBT,
		missRecordSize,
		cudaMemcpyHostToDevice
	));

	CUdeviceptr hitgroupRecord;
	size_t      hitgroupRecordSize = sizeof(HitGroupSbtRecord);
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&hitgroupRecord), hitgroupRecordSize));

	// get the mesh color from the user
	Vector4f col = properties.renderProps->getVal<Vector4f>(RenderKey::MeshColor);
	hitgroupSBT.data.r = col.x();
	hitgroupSBT.data.g = col.y();
	hitgroupSBT.data.b = col.z();

	ProgramGroupHandle mesh = findProgram("mesh_group");
	OPTIX_CHECK(optixSbtRecordPackHeader(mesh->get(), &hitgroupSBT));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(hitgroupRecord),
		&hitgroupSBT,
		hitgroupRecordSize,
		cudaMemcpyHostToDevice
	));

	sbt.raygenRecord = raygenRecord;
	sbt.missRecordBase = missRecord;
	sbt.missRecordStrideInBytes = sizeof(MissSbtRecord);
	sbt.missRecordCount = 1;
	sbt.hitgroupRecordBase = hitgroupRecord;
	sbt.hitgroupRecordStrideInBytes = sizeof(HitGroupSbtRecord);
	sbt.hitgroupRecordCount = 1;

	// safe to delete program groups and modules
	//https://devtalk.nvidia.com/default/topic/1062343/optix/optix-7-module-and-program-group-lifetimes/
	config.programs.programs.clear();
	modules.clear();
}

void OptixScene::syncCamera(CameraHandle& camera)
{
	updateCamera(camera);

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.raygenRecord),
		&raygenSBT,
		sizeof(RayGenSbtRecord),
		cudaMemcpyHostToDevice
	));
}

void OptixScene::updateCamera(CameraHandle& camera)
{
	// recalc the view matrix
	if(camera->isDirty())
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

	raygenSBT.data.cam_eye = camEye;
	raygenSBT.data.camera_u = camRight;
	raygenSBT.data.camera_v = camUp;
	raygenSBT.data.camera_w = camForward;
}

void OptixScene::syncBackgoundColor()
{
	Vector4f bg = properties.renderProps->getVal<Vector4f>(RenderKey::BackgroundColor);

	missSBT.data.r = bg.x();
	missSBT.data.g = bg.y();
	missSBT.data.b = bg.z();

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.missRecordBase),
		&missSBT,
		sizeof(MissSbtRecord),
		cudaMemcpyHostToDevice
	));
}

void OptixScene::syncMeshColor()
{
	Vector4f col = properties.renderProps->getVal<Vector4f>(RenderKey::MeshColor);

	
	//optixSbtRecordPackHeader(config.programs.hitgroupProgs.front()->get(), &hitgroupSBT);

	hitgroupSBT.data.r = col.x();
	hitgroupSBT.data.g = col.y();
	hitgroupSBT.data.b = col.z();

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.hitgroupRecordBase),
		&hitgroupSBT,
		sizeof(HitGroupSbtRecord),
		cudaMemcpyHostToDevice
	));
}
