#include "Jahley.h"
#include <igl/read_triangle_mesh.h>
#include <benchmark.h>

const std::string APP_NAME = "HelloBenchmark";
using juce::String;
using juce::StringArray;
using mace::FileServices;
using igl::read_triangle_mesh;

void loadTriangleMesh( const String & resourceFolder)
{
	StringArray files;
	String wildCard("*.obj;*.ply");
	FileServices::getFiles(resourceFolder, files, wildCard);

	for (auto path : files)
	{
		Eigen::MatrixXd V;
		Eigen::MatrixXi F;
		if (!read_triangle_mesh(path.toStdString(), V, F))
		{
			LOG(CRITICAL) << "load mesh failed for: " << path;
		}
	}

}

static void BM_LoadMesh(benchmark::State& state)
{
	String resourceFolder = getResourcePath("Common");
	
	// Perform setup here
	for (auto _ : state)
	{
		// This code gets timed
		loadTriangleMesh(resourceFolder);
	}
}

// Register the function as a benchmark
BENCHMARK(BM_LoadMesh);


class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings = DesktopWindowSettings(), bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
		int argc = 1;
		std::vector<char*> argv;
		argv.push_back("Hello Benchmark");

		benchmark::Initialize(&argc, &argv[0]);
		benchmark::TimeUnit::kMillisecond; //  how to set this???
		benchmark::RunSpecifiedBenchmarks();
	}

  private:
	
};

Jahley::App* Jahley::CreateApplication()
{
	return new Application();
}
