// This header file was auto-generated by ClassMate++
// Created: 28 Sep 2019 6:48:49 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

#include "optix7_core/optix7_core.h"

class WhittedContext : public OptixRenderContext
{

 public:
	static OptixRenderContextHandle create(PipelineType type) { return std::make_shared<WhittedContext>(type); }

 public:
	WhittedContext (PipelineType type);
	~WhittedContext ();
		
	void createRaygenRecord(const OptixEngineRef& engine) override;
	void createMissRecord(const OptixEngineRef& engine) override;
	void createEmptyHitGroupRecord(const OptixEngineRef& engine) override;

	void rebuildHitgroupSBT(OptixEngineRef& engine) override;
	void updateCamera(CameraHandle& camera) override;
	void initializeLaunchParams() override;

	void preLaunch(CameraHandle& camera, OptixEngineRef& engine, InputEvent& input) override;
	void launch(OptixEngineRef& engine) override;
	void postLaunch(CameraHandle& camera, OptixEngineRef& engine, InputEvent& input) override;

 private:
	CUDAOutputBuffer<uchar4> renderBuffer;
	WhittedParams whittedParams;

	RaygenRecord raygenRecord;
	const size_t raygenRecordSize = sizeof(RaygenRecord);

	ProgramGroupHandle hitProg = nullptr;
	const size_t hitgroup_record_size = sizeof(HitGroupRecord);

	MissRecord missRecord;
	const size_t missRecordSize = sizeof(MissRecord);

}; // end class WhittedContext
