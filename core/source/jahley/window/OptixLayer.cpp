// This source file was auto-generated by ClassMate++
// Created: 17 Aug 2019 3:42:21 pm
// Copyright (c) 2019, HurleyWorks

#include "berserkpch.h"
#include "OptixLayer.h"


// ctor
OptixLayer::OptixLayer (const PropertyService& properties)
	: RenderLayer(properties)
{	
}

// dtor
OptixLayer::~OptixLayer ()
{	
}

void OptixLayer::onUpdate()
{
	//LOG(DBUG) << _FN_;
}

void OptixLayer::onInput(const InputEvent& input)
{
	InputEvent::Type type = input.getType();
	if (type == InputEvent::Type::Press || type == InputEvent::Drag)
	{
		
		//LOG(DBUG) << "Dragging at: " << input.getX() << ", " << input.getY();
	}
}

