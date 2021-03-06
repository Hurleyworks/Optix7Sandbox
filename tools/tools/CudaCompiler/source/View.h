// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks

#pragma once

#include <mace_core/mace_core.h>

class NanoguiLayer;
struct PropertyService;
using juce::StringArray;
using juce::String;

class View : public CsSignal::SignalBase
{

 public:

	SIGNAL_1(void emitConfigName(const std::string& name))
	SIGNAL_2(emitConfigName, name)

	SIGNAL_1(void emitCudaFolder(const std::string& path))
	SIGNAL_2(emitCudaFolder, path)

	SIGNAL_1(void emitIncludeFolder(const std::string& path, bool subfolders))
	SIGNAL_2(emitIncludeFolder, path, subfolders)

	SIGNAL_1(void emitPtxFolder(const std::string& path))
	SIGNAL_2(emitPtxFolder, path)

	SIGNAL_1(void emitCompile())
	SIGNAL_2(emitCompile)

	SIGNAL_1(void emitReset())
	SIGNAL_2(emitReset)

 public:
	View (const PropertyService & properties);
	~View ();
	
	void create(NanoguiLayer* const gui);

	// this fixed crashing in MessageDialog!
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 private:
	 PropertyService properties;
	 NanoguiLayer* gui = nullptr; // owned by App, don't delete

	 // reset button colors
	 int r = 255;
	 int g = 128;
	 int b = 0;
	 int a = 128;

	 // label colors
	 int r1 = 255;
	 int g1 = 100;
	 int b1 = 0;
	 int a1 = 255;

	 bool includeSubfolders = true;
	 String lastSelectedFolder = INVALID_PATH;
	 String configFolder = INVALID_PATH;
	 String cudaFolder = INVALID_PATH;
	 String configName = String::empty;
	 String optix7sdkFolder = "C:\\ProgramData\\NVIDIA Corporation\\OptiX SDK 7.0.0\\SDK";
	 String optixFrameworkFolder = "..\\..\\..\\..\\..\\framework\\optix7_core\\excludeFromBuild";

	 // ugly windows stuff
	 static int CALLBACK browseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	 std::string browseFolder(String saved_path);

}; // end class View
