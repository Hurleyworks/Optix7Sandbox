// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

#include "EngineData.h"

// the bulk of this code is from the Optix7 optixTriangle sample
class TriangleEngine : public OptixEngine
{

 public:
	TriangleEngine (const PropertyService& properties, const OptixConfig& config);
	~TriangleEngine ();

	void init(CameraHandle& camera) override;
	void render(CameraHandle& camera) override
	{
		updateBackgroundColor();
		OptixEngine::render(camera);
	}

 private:
	OptixModule module = nullptr;
	OptixProgramGroup raygen_prog_group = nullptr;
	OptixProgramGroup miss_prog_group = nullptr;
	OptixProgramGroup hitgroup_prog_group = nullptr;

	void createAccelerationStructure(const OptixConfig& config);
	void createModule(const OptixConfig& config, const String& ptxStr);
	void createProgramGroups(const OptixConfig& config);
	void linkPipeline(const OptixConfig& config);
	void setupShaderBindingTable(const OptixConfig& config, CameraHandle& camera);
	void updateCamera(const OptixConfig& config, CameraHandle& camera, RayGenSbtRecord & rg_sbt);

	void updateBackgroundColor();

}; // end class TriangleEngine
