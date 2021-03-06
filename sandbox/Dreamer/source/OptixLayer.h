// This header file was auto-generated by ClassMate++
// Created: 17 Aug 2019 3:42:21 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

#include <optix7_core/optix7_core.h>
#include "window/RenderLayer.h"

class OptixLayer : public RenderLayer
{

 public:
	OptixLayer (const PropertyService& properties, CameraHandle& camera, OptixEngineRef engine);
	~OptixLayer ();

	void onAttach() override;
	void onDetach() override;

	void onUpdate() override;
	bool onInput(const InputEvent& input) override;

	LayerType getType() const override { return LayerType::Optix; }

 private:
	OptixEngineRef engine = nullptr;
	bool attached = false;

}; // end class OptixLayer
