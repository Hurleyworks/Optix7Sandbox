/*
* Copyright (c) 2018 NVIDIA CORPORATION. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*  * Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  * Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*  * Neither the name of NVIDIA CORPORATION nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PtxHandler.h"
#include <nvrtc.h>

using namespace juce;
using namespace mace;

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x
#define LINE_STR STRINGIFY(__LINE__)

// Error check/report helper for users of the C API
#define NVRTC_CHECK_ERROR( func )                                  \
  do {                                                             \
    nvrtcResult code = func;                                       \
    if( code != NVRTC_SUCCESS )                                    \
      throw std::runtime_error( "ERROR: " __FILE__ "(" LINE_STR "): " +     \
          std::string( nvrtcGetErrorString( code ) ) );            \
  } while( 0 )



// Include directories
// here's where the problem starts
// we have to include the paths below because optix_7_types.h includes <stddef.h>
// "C:/Program Files (x86)/Windows Kits/10/Include/10.0.17763.0/ucrt",
// "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.22.27905/include", 

#define INCLUDE_DIRS \
  "C:/Program Files (x86)/Windows Kits/10/Include/10.0.17763.0/ucrt", \
  "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.22.27905/include", \
  "C:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/include", \
  "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.1/include" 
  
 
// NVRTC compiler options
#define CUDA_NVRTC_OPTIONS  \
  "-arch", \
  "compute_30", \
  "-use_fast_math", \
  "-lineinfo", \
  "-default-device", \
  "-rdc", \
  "true", \
  "-std=c++11", \
  "-D__x86_64",

// findCudaFiles
void PtxHandler::findCudaFiles(const std::string & cudaFolder)
{
	StringArray cudaFiles;
	FileServices::getFiles(cudaFolder, cudaFiles, "*.cu");

	for (auto path : cudaFiles)
	{
		File file(path);
		if (!file.existsAsFile()) continue;

		std::cout << "Found cuda file: " << file.getFileName() << std::endl;
		cudaFilePaths.push_back(path.toStdString());
	}
}

// createPtxFilesAndHeaders
void PtxHandler::createPtxFiles(const std::string ptxFolder, bool createHeaders)
{
	ptxOutputFolder = ptxFolder;

	// compile into ptx
	for (auto path : cudaFilePaths)
	{
		compileCudaSource(path);
	}

	if (!createHeaders) return;

	// generate c++ header files containing the ptx code as a string
	StringArray ptxFiles;
	FileServices::getFiles(ptxOutputFolder, ptxFiles, "*.ptx");

	for (auto path : ptxFiles)
	{
		File file(path);
		if (!file.existsAsFile()) continue;

		createPtxHeader(path.toStdString());
	}
}

// createPtxHeaders
void PtxHandler::createPtxHeaders(const std::string & ptxFolder)
{
	StringArray ptxFiles;
	FileServices::getFiles(ptxFolder, ptxFiles, "*.ptx");

	for (auto path : ptxFiles)
	{
		File file(path);
		if (!file.existsAsFile()) continue;

		std::cout << "Found ptx file: " << file.getFileName() << std::endl;
		createPtxHeader(path.toStdString());
	}
}

// compileCudaSource
void PtxHandler::compileCudaSource(const std::string & filePath)
{
	ScopedStopWatch sw(_FN_);

	File file(filePath);
	if (!file.existsAsFile())
		throw std::runtime_error("File does not exist");

	File parent = file.getParentDirectory();
	if(!parent.isDirectory())
		throw std::runtime_error("Parent directory does not exist");

	String s = file.loadFileAsString();
	String routineName = file.getFileNameWithoutExtension();

	LOG(DBUG) << "Compiling " << routineName;

	nvrtcProgram prog = nullptr;
	nvrtcCreateProgram(&prog, s.toStdString().c_str(), nullptr, 0, nullptr, nullptr);
	if (!prog)
		throw std::runtime_error("Could not create nvrt program");

	// Gather NVRTC options
	std::vector<const char *> options;

	// Collect include dirs
	std::vector<std::string> includes;
	String cudaFolder(parent.getFullPathName());
	includes.push_back(std::string("-I") + cudaFolder.toStdString());

	const char *optixFolders[] = { INCLUDE_DIRS };
	const size_t optixFolderCount = sizeof(optixFolders) / sizeof(optixFolders[0]);
	for (size_t i = 0; i < optixFolderCount; i++)
		includes.push_back(std::string("-I") + optixFolders[i]);

	for (std::vector<std::string>::const_iterator it = includes.begin(); it != includes.end(); ++it)
		options.push_back(it->c_str());

	// Collect NVRTC options
	const char* compilerOptions[] = { CUDA_NVRTC_OPTIONS };
	std::copy(std::begin(compilerOptions), std::end(compilerOptions), std::back_inserter(options));

	// JIT compile CU to PTX
	const nvrtcResult compilerResult = nvrtcCompileProgram(prog, (int)options.size(), options.data());

	// Retrieve log output
	size_t logSize = 0;
	nvrtcGetProgramLogSize(prog, &logSize);
	std::string compilerLog;
	compilerLog.resize(logSize);
	if (logSize > 1)
	{
		NVRTC_CHECK_ERROR(nvrtcGetProgramLog(prog, &compilerLog[0]));
	}
	if (compilerResult != NVRTC_SUCCESS)
		throw std::runtime_error("NVRTC Compilation failed.\n" + compilerLog);

	// Retrieve PTX code
	std::string ptx;
	size_t ptxSize = 0;
	NVRTC_CHECK_ERROR(nvrtcGetPTXSize(prog, &ptxSize));
	ptx.resize(ptxSize);
	NVRTC_CHECK_ERROR(nvrtcGetPTX(prog, &ptx[0]));

	juce::String out(ptxOutputFolder + "/" + routineName + ".ptx");
	File f(out);
	f.replaceWithText(String(ptx));

	LOG(DBUG)<< "Saving ptx file: " << f.getFileName() << std::endl;

	// Cleanup
	NVRTC_CHECK_ERROR(nvrtcDestroyProgram(&prog));
}

// createPtxHeader
void PtxHandler::createPtxHeader(const std::string & filePath)
{
	File ptxFile(filePath);
	String ptxString = ptxFile.loadFileAsString();

	// create the header file
	String path(filePath);
	path = path.replace(".ptx", ".h");
	File header(path);
	header.create();

	StringArray out;
	out.add("#pragma once");
	out.add("#define STRINGIFIED(...) #__VA_ARGS__");
	out.add("#define AS_STRING(...) STRINGIFIED(__VA_ARGS__)");

	// use the file name for the variable since it has to be unique
	String variableName = ptxFile.getFileNameWithoutExtension();
	int sections = 0;
	String start = beginSection(variableName, sections++);
	out.add(start);

	StringArray lines = StringArray::fromLines(ptxString);
	int bytesUsed = 0;
	for (auto line : lines)
	{
		bytesUsed += line.length();

		// handle ptx strings that are too big
		// https://msdn.microsoft.com/en-us/library/dddywwsc.aspx
		// Prior to adjacent strings being concatenated, a string cannot be longer than 16380 single-byte characters.
		if (bytesUsed < STRING_BYTE_LIMIT)
		{
			// add this line to the current section
			out.add(line);
		}
		else
		{
			// end this section
			out.add(");");
			bytesUsed = 0;
			
			// start a new section
			String start = beginSection(variableName, sections++);
			out.add(start);

			// add this line to the new section
			out.add(line);
		}
	}
	// end the last section
	out.add(");");

	// concanenate sections 
	String sectionString = "std::string " + variableName +  " = ";
	for (int i = 0; i < sections; i++)
	{
		sectionString += "std::string(" + variableName + String(i) + ")";
		if (i < sections - 1)
			sectionString += " + ";
	}

	out.add(sectionString + ";");

	header.replaceWithText(out.joinIntoString("\n"));

	std::cout << "Created header file: " << header.getFileName() << std::endl;
}

// beginSection
String PtxHandler::beginSection(const String & filename, int section)
{
	String variableName = filename + String(section);

	String firstLine;
	firstLine.append("const char * ", 100);
	firstLine.append(variableName, variableName.length());
	firstLine.append(" = AS_STRING(", 100);

	return firstLine;
}

