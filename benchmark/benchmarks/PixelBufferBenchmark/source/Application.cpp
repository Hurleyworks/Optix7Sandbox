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

	~PixelBufferLight()
	{
		reset();
	}

	PixelBufferLight(const PixelBufferLight& other)
		: spec(other.spec),
		  pixels(other.pixels)
	{
	}

	PixelBufferLight(PixelBufferLight&& other) noexcept
	{
		spec = std::move(other.spec);
		pixels = std::move(other.pixels);
		other.reset();
	}

	void PixelBufferLight::swap(PixelBufferLight& other)
	{
		std::swap(spec, other.spec);
		std::swap(pixels, other.pixels);
	}

	// move assignment only!
	PixelBufferLight& operator = (PixelBufferLight& other) = delete;

	PixelBufferLight& operator = (const PixelBufferLight && other)
	{
		spec = std::move(other.spec);
		pixels = std::move(other.pixels);

		return *this;
	}
	void reset()
	{
		pixels.resize(0, 0);
	}

	ImageInfo spec;
	MatrixXc pixels;
};

struct PixelBufferHeavy
{
	PixelBufferHeavy()
	{
	}

	~PixelBufferHeavy()
	{
		reset();
	}

	PixelBufferHeavy(const PixelBufferHeavy& other)
		: spec(other.spec),
		  pixels(other.pixels),
		  floatPixels(other.floatPixels)
	{
	}

	PixelBufferHeavy(PixelBufferHeavy&& other) noexcept
	{
		spec = std::move(other.spec);
		pixels = std::move(other.pixels);
		floatPixels = std::move(other.floatPixels);

		other.reset();
	}

	void PixelBufferHeavy::swap(PixelBufferHeavy& other)
	{
		std::swap(spec, other.spec);
		std::swap(pixels, other.pixels);
		std::swap(floatPixels, other.floatPixels);
	}

	PixelBufferHeavy& operator = (PixelBufferHeavy other) = delete;

	PixelBufferHeavy& operator = (const PixelBufferHeavy&& other)
	{
		spec = std::move(other.spec);
		pixels = std::move(other.pixels);
		floatPixels = std::move(other.floatPixels);

		return *this;
	}

	void reset()
	{
		pixels.resize(0, 0);
		floatPixels.resize(0, 0);
	}

	ImageInfo spec;
	MatrixXc pixels;
	MatrixXf floatPixels;
};

// ------------------PixelBufferLight
void passByValue(PixelBufferLight pixels)
{
	PixelBufferLight other = pixels;
}

void passByReference(const PixelBufferLight& pixels)
{
	const PixelBufferLight * const other = &pixels;
}

void passByMove(PixelBufferLight&& pixels)
{
	PixelBufferLight && other = std::move(pixels);
}


// ----------------------PixelBufferHeavy
void passByValue( PixelBufferHeavy pixels)
{
	PixelBufferHeavy other = pixels;
}

void passByReference(const PixelBufferHeavy& pixels)
{
	const PixelBufferHeavy * const other = &pixels;
}

void passByMove(PixelBufferHeavy&& pixels)
{
	PixelBufferHeavy && other = std::move(pixels);
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

	// there seems to be virtually no penalty for moving them both?????
	image.floatPixels = MatrixXf::Random(ROWS, TEN_MILLION); 

	// this is the part that gets timed
	for (auto _ : state)
	{
		passByMove(std::move(image));
	}
}
BENCHMARK(BM_passByMoveHeavy);



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
