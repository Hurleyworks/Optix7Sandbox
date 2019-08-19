#include "Jahley.h"

const std::string APP_NAME = "NVCC";
using juce::ChildProcess;
using juce::StringArray;
using juce::String;
using mace::FileServices;

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings = DesktopWindowSettings(), bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
		String resourceFolder = getResourcePath(APP_NAME);
		File res(resourceFolder);
		if (!res.exists())
		{
			LOG(CRITICAL) << "Could not find resource folder at: " << resourceFolder;
			return;
		}

		String cudaFolder = resourceFolder + "/cuda";
		String ptxFolder = resourceFolder + "/ptx/";

		StringArray cudaFiles;
		String wildCard("*.cu*");
		FileServices::getFiles(cudaFolder, cudaFiles, wildCard);

		// compile all the.cu files within this folder
		for (auto cu : cudaFiles)
		{
			File f(cu);
			String ptx = ptxFolder + f.getFileNameWithoutExtension() + ".ptx";
			compile(cu, ptx);
		}
	}

	void compile(const String& cudaFile, const String& ptxFile)
	{
		String nvccExe = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.1/bin/nvcc.exe";
		String help = "--help";
		String fastMath = "-use-fast-math"; // doesn't work.
		String ptx = "-ptx";
		String machine = "-m 64";
		String lang = "-std=c++11"; // doesn't work
		String sdkInclude = "-IC:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/include";
		String sdkRoot = "-IC:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/SDK";
		String util = "-IC:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/SDK/cuda";

		// try some Optix7 SDK samples
		String sample = "-IC:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/SDK/optixPathTracer";
		String sample2 = "-IC:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/SDK/optixRayCasting";
		String sample3 = "-IC:/ProgramData/NVIDIA Corporation/OptiX SDK 7.0.0/SDK/optixSimpleMotionBlur";
		String out = "-o";

		// build the command line
		StringArray args;
		args.add(nvccExe);
		args.add(ptx);
		args.add(sdkInclude);
		args.add(util);
		args.add(sdkRoot);
		args.add(sample);
		args.add(sample2);
		args.add(sample3);
		args.add(machine);
		//args.add(lang); // doesn't work
		//args.add(fastMath); //  doesn't work
		
		args.add(cudaFile);
		args.add(out);
		args.add(ptxFile);

		ChildProcess nvcc;
		nvcc.start(args);

		LOG(DBUG) << nvcc.readAllProcessOutput();
	}
};

Jahley::App* Jahley::CreateApplication()
{
	return new Application();
}
