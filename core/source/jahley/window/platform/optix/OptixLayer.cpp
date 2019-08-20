// This source file was auto-generated by ClassMate++
// Created: 17 Aug 2019 3:42:21 pm
// Copyright (c) 2019, HurleyWorks

#include "berserkpch.h"
#include "OptixLayer.h"

// only once!
#include <optix_function_table_definition.h>

using juce::String;
using juce::File;
using juce::StringArray;
using mace::FileServices;

// ctor
OptixLayer::OptixLayer (const PropertyService& properties)
	: RenderLayer(properties)
{	
	
}

// dtor
OptixLayer::~OptixLayer ()
{	
}

void OptixLayer::onAttach()
{
	context = OptixContext::create();

	std::string resourceFolder = properties.renderProps->getVal<std::string>(RenderKey::ResourceFolder);
	File f(resourceFolder);
	if (!f.exists())
		throw std::runtime_error("No resource folder found at " + resourceFolder);

	String ptxFolder = resourceFolder + "/ptx";
	StringArray ptxFiles;
	String wildCard("*.ptx");
	FileServices::getFiles(ptxFolder, ptxFiles, wildCard);

	for (auto ptx : ptxFiles)
	{
		File f(ptx);
		if (!f.existsAsFile()) continue;

		ModuleHandle mod = OptixMod::create();
		String ptxStr = f.loadFileAsString();
		mod->createFromPtx(context, ptxStr.toStdString());

		modules.push_back(mod);
	}
}

void OptixLayer::onDetach()
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
