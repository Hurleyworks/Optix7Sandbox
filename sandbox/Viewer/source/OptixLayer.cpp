// This source file was auto-generated by ClassMate++
// Created: 17 Aug 2019 3:42:21 pm
// Copyright (c) 2019, HurleyWorks

#include "berserkpch.h"
#include "OptixLayer.h"

// ctor
OptixLayer::OptixLayer (const PropertyService& properties, CameraHandle& camera, OptixEngineRef engine)
	: RenderLayer(properties, camera),
	  engine(engine)
{	
	
}

// dtor
OptixLayer::~OptixLayer ()
{	
}

void OptixLayer::onAttach()
{
	attached = true;
}

void OptixLayer::onDetach()
{
	attached = false;
}

void OptixLayer::onUpdate()
{
	if (engine && attached)
		engine->render(camera);
}

bool OptixLayer::onInput(const InputEvent& input)
{
	InputEvent::Type type = input.getType();

	return false;
}

