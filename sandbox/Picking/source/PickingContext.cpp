// This source file was auto-generated by ClassMate++
// Created: 28 Sep 2019 6:48:49 pm
// Copyright (c) 2019, HurleyWorks

#include "PickingContext.h"

using wabi::Ray3f;
using Eigen::Vector3f;
using Eigen::Vector4f;
using sabi::PRenderableState;

// ctor
PickingContext::PickingContext (PipelineType type)
	: OptixRenderContext(type)
{	
	// preallocate some meshes
	// https://devtalk.nvidia.com/default/topic/1063720/optix/dynamic-scene-building/

	uint32_t count = DEFAULT_PREALLOCATED_MESHES_COUNT * WHITTED_RAY_TYPE_COUNT;
	hitgroup_records.resize(count);

	hitgroup_record_base.alloc_and_upload(hitgroup_records);

	sbt.hitgroupRecordStrideInBytes = static_cast<unsigned int>(hitgroup_record_size);
	sbt.hitgroupRecordCount = static_cast<unsigned int>(hitgroup_records.size());
}

// dtor
PickingContext::~PickingContext ()
{	
}

void PickingContext::createRaygenRecord(const OptixEngineRef& engine)
{
	ProgramGroupHandle raygen = nullptr;

	auto it = config.programs.find(pickGenName);
	if (it != config.programs.end())
		raygen = it->second;
	
	if (!raygen)
		throw std::runtime_error("No pick raygen program found!");

	CUdeviceptr  d_raygenRecord;

	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_raygenRecord), raygenRecordSize));

	OPTIX_CHECK(optixSbtRecordPackHeader(raygen->get(), &raygenRecord));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(d_raygenRecord),
		&raygenRecord,
		raygenRecordSize,
		cudaMemcpyHostToDevice
	));

	sbt.raygenRecord = d_raygenRecord;
}

void PickingContext::createMissRecord(const OptixEngineRef& engine)
{
	ProgramGroupHandle miss = nullptr;
	auto it  = config.programs.find(pickMissName);
	if (it != config.programs.end())
		miss = it->second;

	if (!miss)
		throw std::runtime_error("No pick miss program found!");

	CUdeviceptr d_missRecord;

	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_missRecord), missRecordSize));

	// get the miss color from the user
	Vector4f bg = engine->props().renderProps->getVal<Vector4f>(RenderKey::BackgroundColor);
	missRecord.data.r = bg.x();
	missRecord.data.b = bg.y();
	missRecord.data.g = bg.z();

	OPTIX_CHECK(optixSbtRecordPackHeader(miss->get(), &missRecord));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(d_missRecord),
		&missRecord,
		missRecordSize,
		cudaMemcpyHostToDevice
	));

	sbt.missRecordBase = d_missRecord;
	sbt.missRecordStrideInBytes = missRecordSize;
	sbt.missRecordCount = PickRayType::PICK_RAY_TYPE_COUNT;
}

void PickingContext::createEmptyHitGroupRecord(const OptixEngineRef& engine)
{
	auto it = config.programs.find(pickHitName);
	if (it != config.programs.end())
		hitProg = it->second;

	if (!hitProg)
		throw std::runtime_error("No pick hit program found!");

	for(auto &rec : hitgroup_records)
	{
		OPTIX_CHECK(optixSbtRecordPackHeader(hitProg->get(), &rec));
	}
	hitgroup_record_base.upload(hitgroup_records.data(), hitgroup_records.size());

	sbt.hitgroupRecordBase = hitgroup_record_base.d_pointer();
}

void PickingContext::rebuildHitgroupSBT(OptixEngineRef& engine)
{
	// nothing to do since we're preallocated
}

void PickingContext::updateCamera(CameraHandle& camera)
{
	// No need here since we're sending the pick ray in the launch params
}

void PickingContext::initializeLaunchParams()
{
	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&pickParams.pickBuffer),
		2 * sizeof(uint32_t)
	));

	pickData[0] = ~0; // bitwise not
	pickData[1] = ~0;
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*> (pickParams.pickBuffer),
		pickData,
		2 * sizeof(unsigned int),
		cudaMemcpyHostToDevice
	));

	pickParams.picking = 0;
	deviceParams.alloc(sizeof(pickParams));
	uploadLaunchParameters(pickParams);
}

void PickingContext::launch(OptixEngineRef& engine)
{
	uploadLaunchParameters(pickParams);

	OptixRenderContext::launch(engine);
}

void PickingContext::preLaunch(CameraHandle& camera, OptixEngineRef& engine, InputEvent& input)
{
	pickParams.sceneAccel = engine->getIAS();
	
	// picking is with RMB only and happens during Drag and Press events
	if ((input.getType() == InputEvent::Press || input.getType() == InputEvent::Drag) && input.getButton() == InputEvent::Right)
	{
		const Ray3f& r = input.getPickRay();
		pickParams.picking = 1;
		pickParams.rayOrigin = make_float3(r.origin.x(), r.origin.y(), r.origin.z());
		pickParams.rayDir = make_float3(r.dir.x(), r.dir.y(), r.dir.z());

		//vecStr3f(r.origin, DBUG, "ORIGIN");
		//vecStr3f(r.dir, DBUG, "DIR");

		// to stop the stream of mouse press events
		// if the user keeps the key held down
		input.setType(InputEvent::Idle);
	}
	else
	{
		pickParams.picking = 0;
	}
}

void PickingContext::postLaunch(CameraHandle& camera, OptixEngineRef& engine, InputEvent& input)
{
	if (pickParams.picking)
	{
		deviceParams.download(&pickParams, 1);

		CUDA_CHECK(cudaMemcpy(
			reinterpret_cast<void*> (pickData),
			reinterpret_cast<unsigned int*>(pickParams.pickBuffer),
			2 * sizeof(uint32_t),
			cudaMemcpyDeviceToHost
		));

		OptixMeshHandle mesh = engine->getMeshHandler().findMesh(pickData[0]);
		if (mesh)
		{
			RenderableNode& node = mesh->getNode();
			if (node)
			{
				// toggle selection state
				if (!node->getState().isSelected())
				{
					node->getState().state |= PRenderableState::Selected;
					LOG(DBUG) << "Selecting " << node->getName();
				}
				else
				{
					node->getState().state ^= PRenderableState::Selected;
					LOG(DBUG) << "Deselecting " << node->getName();
				}
				
				engine->getMeshHandler().onSelectionChange(mesh);
				
				engine->setRenderRestart(true);
			}
			LOG(DBUG) << "Picked triangle: " << pickData[1];
		}
	}
}

