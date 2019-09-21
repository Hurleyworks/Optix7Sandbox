// This header file was auto-generated by ClassMate++
// Created: 29 Aug 2019 3:54:59 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

#include <sabi_core/sabi_core.h>

using juce::Result;
using juce::String;

class CompilerConfig : public CsSignal::SlotBase, public CsSignal::SignalBase
{

 public:
	SIGNAL_1(void emitCudaFolder(const std::string& path))
	SIGNAL_2(emitCudaFolder, path)

	SIGNAL_1(void emitIncludeFolder(const std::string& path, bool subfolders))
	SIGNAL_2(emitIncludeFolder, path, subfolders)

	SIGNAL_1(void emitPtxFolder(const std::string& path))
	SIGNAL_2(emitPtxFolder, path)

 public:
	CompilerConfig (const PropertyService& properties);
	~CompilerConfig ();

	void init();

	void loadConfig(const std::string & configName);
	Result saveConfig();

	void setCudaFolder(const std::string& folder);
	void setPtxFolder(const std::string& folder);
	void addIncludeFolder(const std::string& folder);

 private:
	PropertyService properties;
	String configFolder;

	String ptxFolder = String::empty;
	String cudaFolder = String::empty;
	String configName = String::empty;

	// hardwired to just 4 include folders for now
	String includeFolder1 = INVALID_PATH;
	String includeFolder2 = INVALID_PATH;
	String includeFolder3 = INVALID_PATH;
	String includeFolder4 = INVALID_PATH;

}; // end class CompilerConfig
