// This source file was auto-generated by ClassMate++
// Created: 20 Aug 2019 7:49:37 pm
// Copyright (c) 2019, HurleyWorks

#include "Compiler.h"

using mace::FileServices;
using juce::File;
using juce::ChildProcess;

void Compiler::findCudaFiles(const std::string& cudaFolder)
{
	LOG(DBUG) << "Searching for cuda files in " << cudaFolder;
	StringArray files;
	String wildCard("*.cu*");
	FileServices::getFiles(cudaFolder, files, wildCard);

	for (auto path : files)
	{
		File f(path);
		if (f.existsAsFile())
		{
			cudaFilesToCompile.push_back(path.toStdString());
		}
	}
}

void Compiler::addIncludePath(const std::string& includeFolder, bool subfolders)
{
	LOG(DBUG) << "Adding include folder: " << includeFolder;

	File folder(includeFolder);
	if (!folder.exists() || !folder.isDirectory()) return;

	includes.push_back("-I" + includeFolder);

	if (subfolders)
	{
		// include all the other folders within
		StringArray folders;
		FileServices::getFolders(includeFolder, folders);
		for (auto path : folders)
		{
			File f(path);
			if (!f.isDirectory()) continue;
			includes.push_back("-I" + path);
		}
	}
}

void Compiler::runNVCC()
{
	for (auto cu : cudaFilesToCompile)
	{
		File f(cu);
		String ptx = ptxOutputFolder + "/" + f.getFileNameWithoutExtension() + ".ptx";

		LOG(INFO) << "Compling " << f.getFileName();

		compile(cu, ptx);
	}
}

void Compiler::compile(const String& cudaFile, const String& ptxFile)
{
	ScopedStopWatch sw(_FN_);

	args.clear();

	args.add(nvccExe);
	args.add(ptx);
	args.add(fastMath);
	args.add(lineInfo);
	args.add(sdkInclude);
	args.add(util);
	args.add(sdkRoot);

	// add the user added includes
	for (auto path : includes)
	{
		LOG(DBUG) << path;
		args.add(path);
	}

	args.add(cudaFile);
	args.add(out);
	args.add(ptxFile);

	ChildProcess nvcc;
	nvcc.start(args);

	LOG(DBUG) << nvcc.readAllProcessOutput();
}
