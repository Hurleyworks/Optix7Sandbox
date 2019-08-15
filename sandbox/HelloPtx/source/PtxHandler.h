/*

Distributed under the MIT License (MIT)

Copyright (c) 2018 Steve Hurley

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#include <stddef.h>
#include <mace_core/mace_core.h>
#include <cuda_runtime.h>

class PtxHandler
{
	using CudaFilePaths = std::vector<std::string>;

	// https://msdn.microsoft.com/en-us/library/dddywwsc.aspx
	const int STRING_BYTE_LIMIT = 16380;

 public:
	PtxHandler() = default;
	~PtxHandler() = default;
		
	void findCudaFiles(const std::string & cudaFolder);
	size_t getCudaFileCount() const { return cudaFilePaths.size(); }
	void createPtxFiles(const std::string ptxFolder, bool createHeaders); 
	void createPtxHeaders(const std::string & ptxFolder);

 private:
	CudaFilePaths cudaFilePaths;
	juce::String ptxOutputFolder;

	void compileCudaSource(const std::string & filePath);
	void createPtxHeader(const std::string & filePath);
	juce::String beginSection(const juce::String & filename, int section);

}; // end class PtxHandler
