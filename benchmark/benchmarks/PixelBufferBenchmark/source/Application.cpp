#include "Jahley.h"
#include <igl/read_triangle_mesh.h>
#include <benchmark.h>

const std::string APP_NAME = "EigenBenchmark";
using namespace std::chrono_literals;
using namespace Eigen;

const int ONE_THOUSAND = 1000;
const int TEN_THOUSAND = 10000;
const int HUNDRED_THOUSAND = 100000;
const int ONE_MILLION = 1000000;
const int TEN_MILLION = 10000000;
const int ROWS = 3;

struct PixelBufferLight
{
	PixelBufferLight()
	{
	}

	ImageInfo spec;
	MatrixXc pixels;
};

struct PixelBufferHeavy
{
	PixelBufferHeavy()
	{
	}

	ImageInfo spec;
	MatrixXc pixels;

	// looks like no performance hit 
	// having these unitialized matrices here
	MatrixXf pixels1;
	MatrixXf pixels2;
	MatrixXf pixels3;
	MatrixXf pixels4;
	MatrixXf pixels5;
	MatrixXf pixels6;
	MatrixXf pixels7;
	MatrixXf pixels8;
	MatrixXf pixels9;
	MatrixXf pixels10;
};

struct PixelBufferHeavyInit
{
	PixelBufferHeavyInit()
	{
		pixels.resize(0, 0);
		pixels1.resize(0, 0);
		pixels2.resize(0, 0);
		pixels3.resize(0, 0);
		pixels4.resize(0, 0);
		pixels5.resize(0, 0);
		pixels6.resize(0, 0);
		pixels7.resize(0, 0);
		pixels8.resize(0, 0);
		pixels9.resize(0, 0);
		pixels10.resize(0, 0);
	}

	ImageInfo spec;
	MatrixXc pixels;

	// there is a 2x performance hit 
	// even resized to 0;
	MatrixXf pixels1;
	MatrixXf pixels2;
	MatrixXf pixels3;
	MatrixXf pixels4;
	MatrixXf pixels5;
	MatrixXf pixels6;
	MatrixXf pixels7;
	MatrixXf pixels8;
	MatrixXf pixels9;
	MatrixXf pixels10;
};


// ------------------PixelBufferLight
void passByValue(const PixelBufferLight pixels)
{

}

void passByReference(const PixelBufferLight& pixels)
{

}

void passByMove(const PixelBufferLight&& pixels)
{

}


// ----------------------PixelBufferHeavy
void passByValue(const PixelBufferHeavy pixels)
{

}

void passByReference(const PixelBufferHeavy& pixels)
{

}

void passByMove(const PixelBufferHeavy&& pixels)
{

}


// ----------------------PixelBufferHeavyInit
void passByValue(const PixelBufferHeavyInit pixels)
{

}

void passByReference(const PixelBufferHeavyInit& pixels)
{

}

void passByMove(const PixelBufferHeavyInit&& pixels)
{

}

static void BM_passByValue(benchmark::State& state)
{
	PixelBufferLight image;
	image.pixels = MatrixXc::Random(ROWS, TEN_MILLION);

	// this is the part that gets timed
	for (auto _ : state)
	{
		passByValue(image);
	}
}
BENCHMARK(BM_passByValue);

static void BM_passByReference(benchmark::State& state)
{
	PixelBufferLight image;
	image.pixels = MatrixXc::Random(ROWS, TEN_MILLION);

	// this is the part that gets timed
	for (auto _ : state)
	{
		passByReference(image);
	}
}
BENCHMARK(BM_passByReference);

static void BM_passByMove(benchmark::State& state)
{
	PixelBufferLight image;
	image.pixels = MatrixXc::Random(ROWS, TEN_MILLION);

	// this is the part that gets timed
	for (auto _ : state)
	{
		passByMove(std::move(image));
	}
}
BENCHMARK(BM_passByMove);

static void BM_passByValueHeavy(benchmark::State& state)
{
	PixelBufferHeavy image;
	image.pixels = MatrixXc::Random(ROWS, TEN_MILLION);

	// this is the part that gets timed
	for (auto _ : state)
	{
		passByValue(image);
	}
}
BENCHMARK(BM_passByValueHeavy);

static void BM_passByReferenceHeavy(benchmark::State& state)
{
	PixelBufferHeavy image;
	image.pixels = MatrixXc::Random(ROWS, TEN_MILLION);

	// this is the part that gets timed
	for (auto _ : state)
	{
		passByReference(image);
	}
}
BENCHMARK(BM_passByReferenceHeavy);

static void BM_passByMoveHeavy(benchmark::State& state)
{
	PixelBufferHeavy image;
	image.pixels = MatrixXc::Random(ROWS, TEN_MILLION);

	// this is the part that gets timed
	for (auto _ : state)
	{
		passByMove(std::move(image));
	}
}
BENCHMARK(BM_passByMoveHeavy);

static void BM_passByValueHeavyInit(benchmark::State& state)
{
	PixelBufferHeavyInit image;
	image.pixels = MatrixXc::Random(ROWS, TEN_MILLION);

	// this is the part that gets timed
	for (auto _ : state)
	{
		passByValue(image);
	}
}
BENCHMARK(BM_passByValueHeavyInit);

static void BM_passByReferenceHeavyInit(benchmark::State& state)
{
	PixelBufferHeavyInit image;
	image.pixels = MatrixXc::Random(ROWS, TEN_MILLION);

	// this is the part that gets timed
	for (auto _ : state)
	{
		passByReference(image);
	}
}
BENCHMARK(BM_passByReferenceHeavyInit);

static void BM_passByMoveHeavyInit(benchmark::State& state)
{
	PixelBufferHeavyInit image;
	image.pixels = MatrixXc::Random(ROWS, TEN_MILLION);

	// this is the part that gets timed
	for (auto _ : state)
	{
		passByMove(std::move(image));
	}
}
BENCHMARK(BM_passByMoveHeavyInit);

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings = DesktopWindowSettings(), bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
		int argc = 1;
		std::vector<char*> argv;
		argv.push_back("EigenBenchmark");

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
