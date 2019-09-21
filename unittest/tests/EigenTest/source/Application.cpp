#include <sabi_core/sabi_core.h>
#include "Jahley.h"
#include <igl/read_triangle_mesh.h>

const std::string APP_NAME = "EigenTest";

#ifdef CHECK
#undef CHECK
#endif

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

struct Empty {};
struct Base { int a; };
struct Derived : Base { int b; int c; };

using namespace Eigen;
struct A
{
	Vector3f X;
	Vector4f Y;
	Vector3i Z;
};

struct B
{
	Matrix<float, 3, 30> X;
	Matrix<float, 3, 60> Y;
	Matrix<float, 3, 90> Z;
};

struct C
{
	C()
	{
		X.resize(3, 30);
		Y.resize(3, 60);
		Z.resize(3, 90);
	}

	MatrixXf X;
	MatrixXf Y;
	MatrixXf Z;
};

#define PTR2UINT(x) ((unsigned int)(size_t)(x))

using sabi::MeshBuffers;
using sabi::Surface;
using sabi::Surfaces;
using sabi::VMapDB;

using Poco::BinaryWriter;
using Poco::BinaryReader;
using Eigen::Vector3f;
using Eigen::Vector2f;
using igl::read_triangle_mesh;

std::stringstream loadTriangleMesh(const std::string& path)
{
	std::stringstream str;

	MatrixXd V;
	MatrixXi F;
	if (!read_triangle_mesh(path, V, F))
	{
		LOG(CRITICAL) << "load mesh failed for: " << path;
	}

	// libIGL must be transposed
	V.transposeInPlace();
	F.transposeInPlace();

	// pack the indices and vertices into a binary stream
	BinaryWriter writer(str);
	for (int i = 0; i < F.cols(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// must make this assignment first 
			// or it fails for some reason
			// writer << F.col(i)[j]; FAILS
			int index = F.col(i)[j];
			writer << index;
		}
	}

	for (int i = 0; i < V.cols(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// must make this assignment first 
			// or it fails for some reason
			// writer << V.col(i)[j]; FAILS
			float f = V.col(i)[j];
			writer << f;
		}
	}

	return str;
}


TEST_CASE("poco")
{
	std::stringstream str;

	BinaryWriter writer(str);
	writer << true
		<< 'x'
		<< 42
		<< 3.14159265
		<< "foo bar";

	bool   b;
	char   c;
	int    i;
	double d;
	std::string s;

	BinaryReader reader(str);
	reader >> b
		>> c
		>> i
		>> d
		>> s;

	CHECK(b == true);
	CHECK(c == 'x');
	CHECK(i == 42);
	CHECK(d == 3.14159265);
	CHECK(s == "foo bar");
}

TEST_CASE("eigen2binary")
{
	std::stringstream str;
	BinaryWriter writer(str);

	float x = 1.0f;
	float y = 2.0f;
	float z = 3.0f;

	Vector3f v(x, y, z);

	for (int i = 0; i < 3; i++)
		writer << v[i];

	Vector3f v2;
	BinaryReader reader(str);
	for (int i = 0; i < 3; i++)
		reader >> v2[i];

	for (int i = 0; i < 3; i++)
		CHECK(v2[i] == v[i]);
}

TEST_CASE("eigenpack")
{
	std::string resourceFolder = getResourcePath(APP_NAME).toStdString();

	// unit cube centered on origin
	std::stringstream str = loadTriangleMesh(resourceFolder + "/box.obj");

	// should be 36 indices integers and 24 vertex floats
	int totalBytes = 36 * sizeof(int) + 24 * sizeof(float);
	const uint64_t buf_size = str.str().size();

	CHECK(str.str().size() == totalBytes);

	BinaryReader reader(str);

	// should be 12 triangles = 36 integers
	int index;
	for (int i = 0; i < 36; i++)
	{
		reader >> index;

		// indices must be in range of 0-7 ( only 8 vertices )
		CHECK(index >= 0);
		CHECK(index < 8);
	}

	// should be 8 vertices = 24 floats
	float f;
	for (int i = 0; i < 24; i++)
	{
		reader >> f;

		// must be in range of -0.5 and 0.5 since it's a unit cube
		CHECK(f >= -0.5f);
		CHECK(f <= 0.5f);
	}
}


TEST_CASE("[sizeof] testing sizeof")
{
	// https://en.cppreference.com/w/cpp/language/sizeof
	// sizeof cannot be used with function types, incomplete types, or bit - field glvalues.
	// When applied to a reference type, the result is the size of the referenced type.
	// When applied to a class type, the result is the size of an object of that class plus 
	// any additional padding required to place such object in an array.

	
	// When applied to an expression, sizeof does not evaluate the expression,
	// and even if the expression designates a polymorphic object, 
	// the result is the size of the static type of the expression.
	// Lvalue - to - rvalue, array - to - pointer, or function - to - 
	// pointer conversions are not performed.Temporary materialization, 
	// however, is(formally) performed for prvalue arguments : sizeof determines the size of the result object. (since C++17)

	Empty e;
	Derived d;
	Base& b = d;  // this Slicing right? 
	// int j = b.b; // ... no access to Derived members 

	int a[10];

	//The result of sizeof is always nonzero, even if applied to an empty class type.
	CHECK(sizeof e == 1);

	// size of pointer
	CHECK(sizeof & e == 8);

	//size of array of 10 int
	CHECK(sizeof(a) == 40);
	CHECK(sizeof(int[10]) == 40);

	//length of array of 10 int
	CHECK((sizeof a) / (sizeof * a) == 10);
	CHECK((sizeof a) / (sizeof a[0]) == 10);

	//size of the Derived
	CHECK(sizeof d == 12); // 3 ints

	// size of the Derived through Base
	CHECK(sizeof b == 4); // 1 int in Base
}

TEST_CASE("[Eigen] sizeof")
{
	// fixed size
	CHECK(sizeof(Vector4f) == 16);
	CHECK(sizeof(Vector3f) == 12);
	CHECK(sizeof(Vector3i) == 12);
	CHECK(sizeof(Vector2f) == 8);
	CHECK(sizeof(Affine3f) == 64);
	CHECK(sizeof(Matrix4f) == 64);

	// sizeof dynamic matric is 24 bytes?
	CHECK(sizeof(MatrixXf) == 24);

	MatrixXf V;
	const int rows = 3;
	const int cols = 30;
	V.resize(rows, cols);

	for (int i = 0; i < cols; i++)
	{
		V.col(i) = Vector3f::Constant(4.444f);
		CHECK(sizeof(V[i]) == 4); // all 4 byte floats
	}
		
	CHECK(V.rowStride() == 1);
	CHECK(V.colStride() == 3);

	CHECK(V.rows() == rows);
	CHECK(V.cols() == cols);
	CHECK(V.size() == V.rows() * V.cols());

	// check fixed size
	CHECK(sizeof(Matrix<float, rows, cols>) == rows * cols * sizeof(float));

	const int rows2 = 0;
	const int cols2 = 0;
	V.resize(rows2, cols2);

	CHECK(V.rows() == rows2);
	CHECK(V.cols() == cols2);
	CHECK(V.size() == V.rows() * V.cols());

	CHECK(V.rowStride() == 1);
	CHECK(V.colStride() == 0);

	// sizeof is always equal to something even when 0 rows and cols
	//CHECK(sizeof(Matrix<float, rows2, cols2>) == 0); // fails
	CHECK(sizeof(Matrix<float, rows2, cols2>) == 1);
}

TEST_CASE("[MeshBuffer] sizeof")
{
	Vector2i size(100, 100);

	const int pointCount = 4;
	const int triCount = 2;
	const int normalCount = pointCount;

	// sizeof dynamic matrices is always 24
	// even when resized and initialized
	MatrixXu F; // faces
	MatrixXf V; // vertices
	MatrixXf N; // vertex normals

	// vertices
	V.resize(3, pointCount);
	V.col(0) = Vector3f(-size.x(), 0.0f, size.y());
	V.col(1) = Vector3f(size.x(), 0.0f, size.y());
	V.col(2) = Vector3f(size.x(), 0.0f, -size.y());
	V.col(3) = Vector3f(-size.x(), 0.0f, -size.y());

	CHECK(sizeof V == 24);

	// triangles
	F.resize(3, triCount);
	F.col(0) = Vector3u(0, 1, 2);
	F.col(1) = Vector3u(2, 3, 0);

	CHECK(sizeof F == 24);

	// normals
	N.resize(3, pointCount);
	N.col(0) = N.col(1) = N.col(2) = N.col(3) = Vector3f::UnitY();

	CHECK(sizeof N == 24);

	MeshBuffers mesh;
	mesh.V = V;
	mesh.N = N;

	Surface s;
	s.indices() = F;
	mesh.S.push_back(s);

	// compute triangle count by summing all
	// the triangles in each surface
	MatrixXu tris;
	mesh.getAllSurfaceIndices(tris);
	CHECK(mesh.faceCount == triCount);

	MatrixXf* verts = std::addressof(mesh.V);
	CHECK(verts->cols() == pointCount);

	MatrixXf* nrm = std::addressof(mesh.N);
	CHECK(nrm->cols() == pointCount);

	unsigned p = PTR2UINT(&mesh.V);
	unsigned p1 = PTR2UINT(&mesh.N);
	CHECK(sizeof(MatrixXf) == 24); 
	CHECK(p1 - p == 24);

	size_t* faceCount = &mesh.faceCount;
	CHECK(*faceCount == triCount);	

	CHECK(sizeof(MatrixXu) == 24);
	CHECK(sizeof(size_t) == 8);
	CHECK(sizeof(Surfaces) == 32);
	CHECK(sizeof(VMapDB) == 80);

	// MeshBuffers has
	// 2 MatrixXf = 48 bytes
	// 1 Surfaces = 32 bytes;
	// 1 size_t = 8 bytes
	// VMapDB = 80 bytes;
	// total should be 168 bytes
	CHECK(sizeof(MeshBuffers) == 168);
}

// copy eigen dynamic matrix into a std::vector
TEST_CASE("[convert] std vector")
{
	const int pointCount = 1000;
	MatrixXf V;
	V.resize(3, pointCount);

	for (int i = 0; i < pointCount; i++)
	{
		V.col(i) = Vector3f::Random();
	}
	
	std::vector<float> vec(V.size());
	std::memcpy(vec.data(), V.data(), V.size() * sizeof(float));

	auto ptr1 = V.data();
	auto ptr2 = vec.data();

	for (int i = 0; i < V.size(); i++)
	{
		CHECK(*ptr1++ == *ptr2++);
	}
}

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings = DesktopWindowSettings(), bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
		doctest::Context().run();
	}

  private:
	
};

Jahley::App* Jahley::CreateApplication()
{
	return new Application();
}
