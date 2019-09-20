// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

#include "LaunchParams.h"

class OptixScene : public OptixEngine
{
	using RayGenSbtRecord = SbtRecord<RayGenData>;
	using MissSbtRecord = SbtRecord<MissData>;
	using HitGroupSbtRecord = SbtRecord<HitGroupSBT>;

 public:
	OptixScene (const PropertyService& properties, const OptixConfig& config);
	~OptixScene ();

	void init(CameraHandle& camera, const json& groups) override;
	void addRenderable(RenderableNode& node) override;

	void render(CameraHandle& camera) override
	{		
		
		// update app settings on gpu
		syncCamera(camera);
		syncBackgoundColor();
		syncMeshColor();

		renderer->render(camera, OptixEngine::getPtr());

		properties.renderProps->setValue(RenderKey::ResetAccumulator, false);
	}
	
 private:
	RendererHandle renderer = nullptr;
	
	std::vector<OptixMeshHandle> meshes;

	std::vector<HitGroupSbtRecord> hitgroup_records;
	CUdeviceptr deviceIASoutputBuffer = 0;
	CUdeviceptr hitGroupRecordBase = 0;
	size_t hitgroupRecordSize = sizeof(HitGroupSbtRecord);

	// SBT records
	RayGenSbtRecord raygenSBT;
	MissSbtRecord missSBT;
	HitGroupSbtRecord hitgroupSBT;

	void createRaygenRecord(CameraHandle& camera);
	void createMissRecord();
	void createHitRecord();

	void rebuildSceneAccel();
	void updateSBT(OptixMeshHandle mesh);
	void buildSBT(CameraHandle& camera) override;
	void updateCamera(CameraHandle& camera);

	// sync app settings with device
	void syncCamera(CameraHandle& camera);
	void syncBackgoundColor();
	void syncMeshColor();

}; // end class OptixScene
