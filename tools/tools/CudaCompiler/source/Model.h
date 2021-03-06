// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks

#pragma once

#include "Compiler.h"

class Model : public CsSignal::SlotBase
{
 public:
	Model() = default;
	~Model() = default;

	// slots called by View
	void findCudaFiles(const std::string& path) { activeObject.call(&Compiler::findCudaFiles, path); }
	void setPtxOutputFolder(const std::string& path) { activeObject.call(&Compiler::setPtxOutputFolder, path); }
	void addIncludePath(const std::string& includeFolder, bool subfolders) { activeObject.call(&Compiler::addIncludePath, includeFolder, subfolders); }
	void runNVCC() { activeObject.call(&Compiler::runNVCC); }
	void reset() { activeObject.call(&Compiler::reset); }

	void onDrop(const std::vector<std::string>& fileList);
	
 private:
	// run on an ActiveObject so the app remains responsive while compiling
	concurrent<Compiler> activeObject;

}; // end class Model
