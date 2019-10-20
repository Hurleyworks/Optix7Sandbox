// This source file was auto-generated by ClassMate++
// Created: 28 Sep 2019 6:48:49 pm
// Copyright (c) 2019, HurleyWorks

#include "DreamerContext.h"

using sabi::PixelBuffer;
using Eigen::Vector4f;
using Eigen::Vector3f;
using wabi::Ray3f;
using sabi::PRenderableState;

// ctor
DreamerContext::DreamerContext (PipelineType type)
	: OptixRenderContext(type)
{	
	sbt.hitgroupRecordStrideInBytes = static_cast<unsigned int>(hitgroup_record_size);
}

// dtor
DreamerContext::~DreamerContext ()
{	
}

void DreamerContext::createRaygenRecord(const OptixEngineRef& engine)
{
	ProgramGroupHandle raygen = nullptr;

	auto it = config.programs.find(raygenName);
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

void DreamerContext::createMissRecord(const OptixEngineRef& engine)
{
	ProgramGroupHandle miss = nullptr;
	auto it = config.programs.find(radianceMissName);
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
	sbt.missRecordCount = WhitedRayType::WHITTED_RAY_TYPE_COUNT;
}

void DreamerContext::createEmptyHitGroupRecord(const OptixEngineRef& engine)
{
	hitProg = nullptr;

	auto it = config.programs.find(radianceHitName);
	if (it != config.programs.end())
		hitProg = it->second;

	if (!hitProg)
		throw std::runtime_error("No hit program found!");

	HitGroupRecord rec = {};
	memset(&rec, 0, hitgroup_record_size);

	OPTIX_CHECK(optixSbtRecordPackHeader(hitProg->get(), &rec));
	
	sbt.hitgroupRecordBase = engine->getMeshHandler().getHitGroupRecordBase();
	sbt.hitgroupRecordCount = static_cast<unsigned int>(engine->getMeshHandler().getRecordCount());
}

void DreamerContext::rebuildHitgroupSBT(OptixEngineRef& engine)
{

}

void DreamerContext::launch(OptixEngineRef& engine)
{
	systemData.topObject = engine->getIAS();

	// if the scene has changed we need 
	// to resart the accumulator by setting
	// the params.subframe_index  to 0
	if (engine->restartRender())
	{
		systemData.subframe_index = 0;
		engine->setRenderRestart(false);
	}
	else
	{
		systemData.subframe_index++;
	}

	uploadLaunchParameters(systemData);
   
	OptixRenderContext::launch(engine);
}

void DreamerContext::updateCamera(CameraHandle& camera)
{
	if (!camera->isDirty()) return;
	
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

	raygenRecord.data.cam_eye = camEye;
	raygenRecord.data.camera_u = camRight;
	raygenRecord.data.camera_v = camUp;
	raygenRecord.data.camera_w = camForward;

	// sync to device
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.raygenRecord),
		&raygenRecord,
		sizeof(RaygenRecord),
		cudaMemcpyHostToDevice
	)); 
}

void DreamerContext::initializeLaunchParams()
{
	// rendering
	colorBuffer.resize(size.x() * size.y() * sizeof(uchar4));
	systemData.outputBuffer = colorBuffer.d_pointer();
	systemData.resolution = make_int2(size.x(), size.y());

	// create the accumulation buffer
	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&systemData.accum_buffer),
		size.x()* size.y() * sizeof(float4)
	));

	// picking
	pickingBuffer.resize(2 * sizeof(uint32_t));
	systemData.pickingBuffer = pickingBuffer.d_pointer();
	systemData.pickingEnabled = 0;
	systemData.pickingFragment = make_float2(-1.0f, -1.0f);

	systemData.subframe_index = 0u;

	const float loffset = 1.05f; //  scene.aabb().maxExtent();

	/// TODO: add light support to sutil::Scene
	std::vector<OptixLight::Point> lights(2);
	lights[0].color = { 1.0f, 1.0f, 0.9f };
	lights[0].intensity = 5.0f;
	lights[0].position = make_float3(10.0f, 10.0f, 10.0f);
	lights[0].falloff = OptixLight::Falloff::NONE; // not used in the shader anyway
	lights[1].color = { 0.8f, 0.9f, 1.0f };
	lights[1].intensity = 3.0f;
	lights[1].position = make_float3(-10.0f, 10.0f, -10.0f);
	lights[1].falloff = OptixLight::Falloff::NONE; // not used in the shader anyway

	systemData.lights.count = static_cast<uint32_t>(lights.size());
	CUDA_CHECK(cudaMalloc(
		reinterpret_cast<void**>(&systemData.lights.data),
		lights.size() * sizeof(OptixLight::Point)
	));

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(systemData.lights.data),
		lights.data(),
		lights.size() * sizeof(OptixLight::Point),
		cudaMemcpyHostToDevice
	));

	deviceParams.alloc(sizeof(systemData));
	uploadLaunchParameters(systemData);
}

bool DreamerContext::preLaunch(CameraHandle& camera, OptixEngineRef& engine, InputEvent& input)
{
	// sync background color from user
	Vector4f bg = engine->props().renderProps->getVal<Vector4f>(RenderKey::BackgroundColor);

	missRecord.data.r = bg.x();
	missRecord.data.g = bg.y();
	missRecord.data.b = bg.z();

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.missRecordBase),
		&missRecord,
		sizeof(MissRecord),
		cudaMemcpyHostToDevice
	));

	bool  picking = false;

	// picking is with RMB only and happens during Drag and Press events
	if ((input.getType() == InputEvent::Press || input.getType() == InputEvent::Drag) && input.getButton() == InputEvent::Right)
	{
		systemData.pickingEnabled = 1;

		// OptiX launch index 0,0 is *bottom* left
		// so subtract Y coord from screen height
		float y = camera->getScreenHeight() - input.getY();
		systemData.pickingFragment = make_float2(input.getX(), y);

		// to stop the stream of mouse press events
		// if the user keeps the key held down
		input.setType(InputEvent::Idle);

		picking = true;
	}
	else
	{
		systemData.pickingEnabled = 0;
		systemData.pickingFragment = make_float2(-1.0f, -1.0f);
	}

	return picking;
}

void DreamerContext::postLaunch(CameraHandle& camera, OptixEngineRef& engine, InputEvent& input)
{
	// acquire the Optix render 
	PixelBuffer& pixels = camera->getPixelBuffer();
	colorBuffer.download(pixels.uint8Pixels.data(),pixels.uint8Pixels.size());

	// select any picked meshes
	if (systemData.pickingEnabled)
	{
		deviceParams.download(&systemData, 1);

		CUDA_CHECK(cudaMemcpy(
			reinterpret_cast<void*> (pickData),
			reinterpret_cast<unsigned int*>(systemData.pickingBuffer),
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

	// reset 
	systemData.pickingEnabled = 0;
	systemData.pickingFragment = make_float2(-1.0f, -1.0f);
	size = Vector3i(DEFAULT_DESKTOP_WINDOW_WIDTH, DEFAULT_DESKTOP_WINDOW_HEIGHT, 1);
}

