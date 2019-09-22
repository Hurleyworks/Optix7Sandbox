// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks

#pragma once

#include <sabi_core/sabi_core.h>

class NanoguiLayer;
struct PropertyService;
using juce::StringArray;
using sabi::PrimitiveType;
using sabi::MeshOptions;
using Eigen::Vector2f;

class View : public CsSignal::SignalBase
{ 
	// https://www.bogotobogo.com/DesignPatterns/mvc_model_view_controller_pattern.php
	// Present the Model to the user in an appropriate interface
	// Allows user to manipulate data through the Controller
	// Does not store any data except to cache state
	// Easily reusable & configurable to display different data

public:
	SIGNAL_1(void emitPrimitiveType(PrimitiveType type, MeshOptions options))
	SIGNAL_2(emitPrimitiveType, type, options)

	SIGNAL_1(void emitModelPath(const std::string& path))
	SIGNAL_2(emitModelPath, path)

	SIGNAL_1(void emitGroundPlane(const Vector2f& size))
	SIGNAL_2(emitGroundPlane, size)

	SIGNAL_1(void emitFrameGrab())
	SIGNAL_2(emitFrameGrab)

	SIGNAL_1(void emitScreenGrab())
	SIGNAL_2(emitScreenGrab)

	SIGNAL_1(void emitClearScene())
	SIGNAL_2(emitClearScene)

 public:
	View (const PropertyService& properties);
	~View ();
	
	void create(NanoguiLayer* const gui, const std::string& appName);

	// this fixed crashing in MessageDialog!
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 private:
	 PropertyService properties;
	 NanoguiLayer* gui = nullptr; // owned by App, don't delete
	 MeshOptions meshOptions = MeshOptions::CenterVertices | MeshOptions::NormalizeSize | MeshOptions::RestOnGround | MeshOptions::LoadStrategy;
	 std::string commonFolder;
	 std::vector<std::pair<int, std::string>> modelIcons;

	 // label colors
	 int r1 = 255;
	 int g1 = 100;
	 int b1 = 0;
	 int a1 = 255;

	 const StringArray getSceneInfo();

}; // end class View
