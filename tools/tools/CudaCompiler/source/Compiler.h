// This header file was auto-generated by ClassMate++
// Created: 20 Aug 2019 7:49:37 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

#include <mace_core/mace_core.h>
#include <concurrent.hpp>

using juce::String;
using juce::StringArray;

class Compiler
{
	using CudaFIles = std::vector<String>;
	using IncludeFolders = std::vector<String>;

 public:
	Compiler() = default;
	~Compiler() = default;
		
	void findCudaFiles(const std::string& path);
	void setPtxOutputFolder(const std::string& path) { ptxOutputFolder = path; }
	void addIncludePath(const std::string& includeFolder, bool subfolders);
	void runNVCC();
	void reset()
	{
		cudaFilesToCompile.clear();
		includes.clear();
		ptxOutputFolder = INVALID_PATH;
		args.clear();
	}

 private:
	String ptxOutputFolder = INVALID_PATH;
	CudaFIles cudaFilesToCompile;
	IncludeFolders includes;

	StringArray args;
	String nvccExe = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.1/bin/nvcc.exe";
	String help = "--help";
	String fastMath = "-use_fast_math"; // doesn't work.
	String ptx = "-ptx";
	String lineInfo = "-lineinfo";
	String machine = "-m 64";
	String lang = "-std=c++11"; // doesn't work
	String sdkInclude = "-IC:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/include";
	String sdkRoot = "-IC:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/SDK";
	String util = "-IC:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/SDK/cuda";
	String out = "-o";

	void compile(const String& cudaFile, const String& ptxFile);

}; // end class Compiler
