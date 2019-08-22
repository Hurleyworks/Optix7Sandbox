// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks

#pragma once

#include <sabi_core/sabi_core.h>

class NanoguiLayer;
struct PropertyService;
using juce::StringArray;
using sabi::PrimitiveType;

class View : public CsSignal::SignalBase
{ 
	// https://www.bogotobogo.com/DesignPatterns/mvc_model_view_controller_pattern.php
	// Present the Model to the user in an appropriate interface
	// Allows user to manipulate data through the Controller
	// Does not store any data except to cache state
	// Easily reusable & configurable to display different data

public:
	SIGNAL_1(void emitPrimitiveType(PrimitiveType type))
	SIGNAL_2(emitPrimitiveType, type)

 public:
	View (const PropertyService & properties);
	~View ();
	
	void create(NanoguiLayer* const gui);

	// this fixed crashing in MessageDialog!
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 private:
	 PropertyService properties;
	 NanoguiLayer* gui = nullptr; // owned by App, don't delete
	
	 // label colors
	 int r1 = 255;
	 int g1 = 100;
	 int b1 = 0;
	 int a1 = 255;

	 const StringArray getSceneInfo();

}; // end class View
