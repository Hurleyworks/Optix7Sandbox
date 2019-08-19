#include "Jahley.h"

const std::string APP_NAME = "HelloBenchmark";

#include <benchmark.h>

void testThis()
{

}

static void BM_SomeFunction(benchmark::State& state)
{
	// Perform setup here
	for (auto _ : state)
	{
		// This code gets timed
		testThis();
	}
}

// Register the function as a benchmark
BENCHMARK(BM_SomeFunction);


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
		benchmark::RunSpecifiedBenchmarks();
	}

  private:
	
};

Jahley::App* Jahley::CreateApplication()
{
	return new Application();
}
