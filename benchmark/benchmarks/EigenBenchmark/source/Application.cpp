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

// Eigen vs std::vector

static void Random_EIGEN_ONE_THOUSAND(benchmark::State& state)
{
	for (auto _ : state)
	{
		MatrixXf m = MatrixXf::Random(ROWS, ONE_THOUSAND);
	}
}
BENCHMARK(Random_EIGEN_ONE_THOUSAND);

static void Random_Vector_ONE_THOUSAND(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<float> v(ONE_THOUSAND * ROWS);
		std::generate(v.begin(), v.end(), std::rand);
	}
}
BENCHMARK(Random_Vector_ONE_THOUSAND);


static void Random_EIGEN_TEN_THOUSAND(benchmark::State& state)
{
	for (auto _ : state)
	{
		MatrixXf m = MatrixXf::Random(ROWS, TEN_THOUSAND);
	}
}
BENCHMARK(Random_EIGEN_TEN_THOUSAND);


static void Random_Vector_TEN_THOUSAND(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<float> v(ROWS * TEN_THOUSAND);
		std::generate(v.begin(), v.end(), std::rand);
	}
}
BENCHMARK(Random_Vector_TEN_THOUSAND);

static void Random_EIGEN_HUNDRED_THOUSAND(benchmark::State& state)
{
	for (auto _ : state)
	{
		MatrixXf m = MatrixXf::Random(ROWS, HUNDRED_THOUSAND);
	}
}
BENCHMARK(Random_EIGEN_HUNDRED_THOUSAND);


static void Random_Vector_HUNDRED_THOUSAND(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<float> v(ROWS * HUNDRED_THOUSAND);
		std::generate(v.begin(), v.end(), std::rand);
	}
}
BENCHMARK(Random_Vector_HUNDRED_THOUSAND);

static void Random_EIGEN_ONE_MILLION(benchmark::State& state)
{
	for (auto _ : state)
	{
		MatrixXf m = MatrixXf::Random(ROWS, ONE_MILLION);
	}
}
BENCHMARK(Random_EIGEN_ONE_MILLION);


static void Random_Vector_ONE_MILLION(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<float> v(ROWS * ONE_MILLION);
		std::generate(v.begin(), v.end(), std::rand);
	}
}
BENCHMARK(Random_Vector_ONE_MILLION);

static void Random_EIGEN_TEN_MILLION(benchmark::State& state)
{
	for (auto _ : state)
	{
		MatrixXf m = MatrixXf::Random(ROWS, TEN_MILLION);
	}
}
BENCHMARK(Random_EIGEN_TEN_MILLION);


static void Random_Vector_TEN_MILLION(benchmark::State& state)
{
	for (auto _ : state)
	{
		std::vector<float> v(TEN_MILLION * ROWS);
		std::generate(v.begin(), v.end(), std::rand);
	}
}
BENCHMARK(Random_Vector_TEN_MILLION);

// ----------------- copy
static void Random_EIGEN_Copy_ONE_THOUSAND(benchmark::State& state)
{
	MatrixXf m1 = MatrixXf::Random(ROWS, ONE_THOUSAND);
	MatrixXf m2 = MatrixXf::Random(ROWS, ONE_THOUSAND);
	for (auto _ : state)
	{
		m1 = m2;
	}
}
BENCHMARK(Random_EIGEN_Copy_ONE_THOUSAND);


static void Random_Vector_Copy_ONE_THOUSAND(benchmark::State& state)
{
	std::vector<float> v1(ONE_THOUSAND * ROWS);
	std::generate(v1.begin(), v1.end(), std::rand);

	std::vector<float> v2(ONE_THOUSAND * ROWS);
	std::generate(v2.begin(), v2.end(), std::rand);

	for (auto _ : state)
	{
		v1 = v2;
	}
}
BENCHMARK(Random_Vector_Copy_ONE_THOUSAND);

static void Random_EIGEN_Copy_TEN_THOUSAND(benchmark::State& state)
{
	MatrixXf m1 = MatrixXf::Random(ROWS, TEN_THOUSAND);
	MatrixXf m2 = MatrixXf::Random(ROWS, TEN_THOUSAND);
	for (auto _ : state)
	{
		m1 = m2;
	}
}

BENCHMARK(Random_EIGEN_Copy_TEN_THOUSAND);

static void Random_Vector_Copy_TEN_THOUSAND(benchmark::State& state)
{
	std::vector<float> v1(TEN_THOUSAND * ROWS);
	std::generate(v1.begin(), v1.end(), std::rand);

	std::vector<float> v2(TEN_THOUSAND * ROWS);
	std::generate(v2.begin(), v2.end(), std::rand);

	for (auto _ : state)
	{
		v1 = v2;
	}
}
BENCHMARK(Random_Vector_Copy_TEN_THOUSAND);

static void Random_EIGEN_Copy_HUNDRED_THOUSAND(benchmark::State& state)
{
	MatrixXf m1 = MatrixXf::Random(ROWS, HUNDRED_THOUSAND);
	MatrixXf m2 = MatrixXf::Random(ROWS, HUNDRED_THOUSAND);

	for (auto _ : state)
	{
		m1 = m2;
	}
}
BENCHMARK(Random_EIGEN_Copy_HUNDRED_THOUSAND);

static void Random_Vector_Copy_HUNDRED_THOUSAND(benchmark::State& state)
{
	std::vector<float> v1(HUNDRED_THOUSAND * ROWS);
	std::generate(v1.begin(), v1.end(), std::rand);

	std::vector<float> v2(HUNDRED_THOUSAND * ROWS);
	std::generate(v2.begin(), v2.end(), std::rand);

	for (auto _ : state)
	{
		v1 = v2;
	}
}
BENCHMARK(Random_Vector_Copy_HUNDRED_THOUSAND);

static void Random_EIGEN_Copy_ONE_MILLION(benchmark::State& state)
{
	MatrixXf m1 = MatrixXf::Random(ROWS, ONE_MILLION);
	MatrixXf m2 = MatrixXf::Random(ROWS, ONE_MILLION);

	for (auto _ : state)
	{
		m1 = m2;
	}
}
BENCHMARK(Random_EIGEN_Copy_ONE_MILLION);


static void Random_Vector_Copy_ONE_MILLION(benchmark::State& state)
{
	std::vector<float> v1(ONE_MILLION * ROWS);
	std::generate(v1.begin(), v1.end(), std::rand);

	std::vector<float> v2(ONE_MILLION * ROWS);
	std::generate(v2.begin(), v2.end(), std::rand);

	for (auto _ : state)
	{
		v1 = v2;
	}
}
BENCHMARK(Random_Vector_Copy_ONE_MILLION);

static void Random_EIGEN_Copy_TEN_MILLION(benchmark::State& state)
{
	MatrixXf m1 = MatrixXf::Random(ROWS, TEN_MILLION);
	MatrixXf m2 = MatrixXf::Random(ROWS, TEN_MILLION);
	for (auto _ : state)
	{
		m1 = m2;
	}
}
BENCHMARK(Random_EIGEN_Copy_TEN_MILLION);


static void Random_Vector_Copy_TEN_MILLION(benchmark::State& state)
{
	std::vector<float> v1(TEN_MILLION * ROWS);
	std::generate(v1.begin(), v1.end(), std::rand);

	std::vector<float> v2(TEN_MILLION * ROWS);
	std::generate(v2.begin(), v2.end(), std::rand);

	for (auto _ : state)
	{
		v1 = v2;
	}
}
BENCHMARK(Random_Vector_Copy_TEN_MILLION);


// ----------------- move

static void Random_EIGEN_Move_ONE_THOUSAND(benchmark::State& state)
{
	MatrixXf m1 = MatrixXf::Random(ROWS, ONE_THOUSAND);
	MatrixXf m2 = MatrixXf::Random(ROWS, ONE_THOUSAND);

	for (auto _ : state)
	{
		m1 = std::move(m2);
	}
}
BENCHMARK(Random_EIGEN_Move_ONE_THOUSAND);


static void Random_Vector_Move_ONE_THOUSAND(benchmark::State& state)
{
	std::vector<float> v1(ONE_THOUSAND * ROWS);
	std::generate(v1.begin(), v1.end(), std::rand);

	std::vector<float> v2(ONE_THOUSAND * ROWS);
	std::generate(v2.begin(), v2.end(), std::rand);

	for (auto _ : state)
	{
		v1 = std::move(v2);
	}
}
BENCHMARK(Random_Vector_Move_ONE_THOUSAND);


static void Random_EIGEN_Move_TEN_THOUSAND(benchmark::State& state)
{
	MatrixXf m1 = MatrixXf::Random(ROWS, TEN_THOUSAND);
	MatrixXf m2 = MatrixXf::Random(ROWS, TEN_THOUSAND);

	for (auto _ : state)
	{
		m1 = std::move(m2);
	}
}
BENCHMARK(Random_EIGEN_Move_TEN_THOUSAND);


static void Random_Vector_Move_TEN_THOUSAND(benchmark::State& state)
{
	std::vector<float> v1(TEN_THOUSAND * ROWS);
	std::generate(v1.begin(), v1.end(), std::rand);

	std::vector<float> v2(TEN_THOUSAND * ROWS);
	std::generate(v2.begin(), v2.end(), std::rand);

	for (auto _ : state)
	{
		v1 = std::move(v2);
	}
}
BENCHMARK(Random_Vector_Move_TEN_THOUSAND);

static void Random_EIGEN_Move_HUNDRED_THOUSAND(benchmark::State& state)
{
	MatrixXf m1 = MatrixXf::Random(ROWS, HUNDRED_THOUSAND);
	MatrixXf m2 = MatrixXf::Random(ROWS, HUNDRED_THOUSAND);

	for (auto _ : state)
	{
		m1 = std::move(m2);
	}
}

BENCHMARK(Random_EIGEN_Move_HUNDRED_THOUSAND);

static void Random_Vector_Move_HUNDRED_THOUSAND(benchmark::State& state)
{
	std::vector<float> v1(HUNDRED_THOUSAND * ROWS);
	std::generate(v1.begin(), v1.end(), std::rand);

	std::vector<float> v2(HUNDRED_THOUSAND * ROWS);
	std::generate(v2.begin(), v2.end(), std::rand);

	for (auto _ : state)
	{
		v1 = std::move(v2);
	}
}
BENCHMARK(Random_Vector_Move_HUNDRED_THOUSAND);

static void Random_EIGEN_Move_ONE_MILLION(benchmark::State& state)
{
	MatrixXf m1 = MatrixXf::Random(ROWS, ONE_MILLION);
	MatrixXf m2 = MatrixXf::Random(ROWS, ONE_MILLION);

	for (auto _ : state)
	{
		m1 = std::move(m2);
	}
}
BENCHMARK(Random_EIGEN_Move_ONE_MILLION);

static void Random_Vector_Move_ONE_MILLION(benchmark::State& state)
{
	std::vector<float> v1(ONE_MILLION * ROWS);
	std::generate(v1.begin(), v1.end(), std::rand);

	std::vector<float> v2(ONE_MILLION * ROWS);
	std::generate(v2.begin(), v2.end(), std::rand);

	for (auto _ : state)
	{
		v1 = std::move(v2);
	}
}
BENCHMARK(Random_Vector_Move_ONE_MILLION);

static void Random_EIGEN_Move_TEN_MILLION(benchmark::State& state)
{
	MatrixXf m1 = MatrixXf::Random(ROWS, TEN_MILLION);
	MatrixXf m2 = MatrixXf::Random(ROWS, TEN_MILLION);

	for (auto _ : state)
	{
		m1 = std::move(m2);
	}
}
BENCHMARK(Random_EIGEN_Move_TEN_MILLION);

static void Random_Vector_Move_TEN_MILLION(benchmark::State& state)
{
	std::vector<float> v1(TEN_MILLION * ROWS);
	std::generate(v1.begin(), v1.end(), std::rand);

	std::vector<float> v2(TEN_MILLION * ROWS);
	std::generate(v2.begin(), v2.end(), std::rand);

	for (auto _ : state)
	{
		v1 = std::move(v2);
	}
}
BENCHMARK(Random_Vector_Move_TEN_MILLION);



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
