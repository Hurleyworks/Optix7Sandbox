﻿// This header file was auto-generated by ClassMate++
// Created: 8 May 2015 5:35:26 pm
// Copyright (c) 2015, HurleyWorks

#pragma once

struct Vert
{
	float Position[4];
	float Color[4];
};

static void vecStr3f(const Eigen::Vector3f & v, LEVELS level, const char function[] = "unknown function")
{
	LOG(level) << "--- called from " << function;

	std::ostringstream buffer;
	buffer.setf(std::ios::fixed, std::ios::floatfield);
	buffer.precision(6);
	buffer << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";

	LOG(level) << buffer.str();
}

static void vecStr3d(const Eigen::Vector3d & v, LEVELS level, const char function[] = "unknown function")
{
	LOG(level) << "--- called from " << function;

	std::ostringstream buffer;
	buffer.setf(std::ios::fixed, std::ios::floatfield);
	buffer.precision(8);
	buffer << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";

	LOG(level) << buffer.str();
}

static void vecStr4f(const Eigen::Vector4f & v, LEVELS level, const char function[] = "unknown function")
{
	std::ostringstream buffer;
	buffer.setf(std::ios::fixed, std::ios::floatfield);
	buffer.precision(6);
	buffer << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";

	LOG(level) << buffer.str();
}

#if 0
static void matStr4f(const Eigen::Affine3f & t, LEVELS level, const char function[] = "unknown function")
{
	LOG(level) << "--- called from " << function;

	for (int i = 0; i < 4; i++)
		vecStr4f(t.matrix().row(i), level, function);
}

static void matStr4f(const Eigen::Matrix4f & m, LEVELS level, const char function[] = "unknown function")
{
	LOG(level) << "--- called from " << function;

	for (int i = 0; i < 4; i++)
		vecStr4f(m.row(i), level, function);
}

static void matStr3f(const Eigen::Matrix3f & m, LEVELS level, const char function[] = "unknown function")
{
	LOG(level) << "--- called from " << function;

	std::ostringstream buffer;
	buffer.setf(std::ios::fixed, std::ios::floatfield);
	buffer.precision(6);

	for (int i = 0; i < 3; i++)
	{
		Eigen::Vector3f v = m.row(i);
		buffer << "\n\t\t|" << v[0] << ", " << v[1] << ", " << v[2] << "|";
	}

	LOG(level) << buffer.str();
}

static void boxStr3f(BBox3f & box, LEVELS level, const char function[] = "unknown function")
{
	vecStr3f(box.min(), level, function);
	vecStr3f(box.max(), level, function);
}

#endif
#if 0
template <class T>
static bool isOrthogonal(const Eigen::Matrix<T, 3, 1>  & m)
{
	// to test whether a matrix is orthogonal, mutliply the matrix by 
	// it's transform and compare to identity matrix
	return (m * m.transpose()).isIdentity();
}
#endif

#if 1
template <class T>
static bool isOrthogonal(const Eigen::Matrix<T, 3, 3>  & m)
{
	// to test whether a matrix is orthogonal, mutliply the matrix by 
	// it's transform and compare to identity matrix
	return (m * m.transpose()).isIdentity();
}
#endif

static bool isNotValid(const Eigen::Matrix3f & m)
{
	return m.hasNaN();
}

static float infinity()
{
	return std::numeric_limits<float>::infinity();
}

template <class T>
inline T clamp(T a, T b, T c)
{
	return (a < b) ? b : ((a > c) ? c : a);
}

// rad_to_deg
template <typename T>
inline T rad_to_deg(const T rad)
{
	return rad * 180 / Math<T>::PI;
}

// deg_to_rad
template <typename T>
inline T deg_to_rad(const T deg)
{
	return deg * Math<T>::PI / 180;
}

// generates a transform matrix with v as the z axis, taken from PBRT
inline void basisFromVector(const Eigen::Vector3f & w, Eigen::Vector3f* u, Eigen::Vector3f* v)
{
	if (fabsf(w.x()) > fabsf(w.y()))
	{
		float invLen = 1.0f / sqrtf(w.x()*w.x() + w.z()*w.z());
		*u = Eigen::Vector3f(-w.z()*invLen, 0.0f, w.x()*invLen);
	}
	else
	{
		float invLen = 1.0f / sqrtf(w.y()*w.y() + w.z()*w.z());
		*u = Eigen::Vector3f(0.0f, w.z()*invLen, -w.y()*invLen);
	}

	*v = w.cross(*u);

	jassert(fabsf((*u).norm() - 1.0f) < 0.01f);
	jassert(fabsf((*v).norm() - 1.0f) < 0.01f);
}


// this is designed as a drop in replacement for gluPerspective
inline Eigen::Matrix4f projectionMatrix(float fov, float aspect, float znear, float zfar)
{
	float f = 1.0f / tanf(deg_to_rad(fov*0.5f));
	float zd = znear - zfar;

	float view[4][4] = { { f / aspect, 0.0f, 0.0f, 0.0f },
	{ 0.0f, f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, (zfar + znear) / zd, -1.0f },
	{ 0.0f, 0.0f, (2.0f*znear*zfar) / zd, 0.0f } };

	return Eigen::Matrix4f(&view[0][0]);
}

#if 1
inline bool reOrthogonalize(Eigen::Matrix3f & m)
{
	// http://stackoverflow.com/questions/23080791/eigen-re-orthogonalization-of-rotation-matrix

	Eigen::Matrix3f mo = m;

	Eigen::Vector3f x = mo.row(0);
	Eigen::Vector3f y = mo.row(1);
	Eigen::Vector3f z = mo.row(2);

	float error = x.dot(y);

	Eigen::Vector3f x_ort = x - (error / 2)*y;
	Eigen::Vector3f y_ort = y - (error / 2)*x;
	Eigen::Vector3f z_ort = x_ort.cross(y_ort);

	mo.row(0) = x_ort.normalized();
	mo.row(1) = y_ort.normalized();
	mo.row(2) = z_ort.normalized();

	if (isOrthogonal(mo))
	{
		m = mo;
		return true;
	}
	else
	{
		return false;
	}
}

#endif

// randomSymmetry
inline double randomSymmetry(int max)
{
	mace::pcg32 r;
	double n = r.nextDouble();
	return (2.0 * (double)max * n) - (double)max;
}

template<typename T>
inline T dotPerp(const Eigen::Matrix<T, 2, 1> & vec1, const Eigen::Matrix<T, 2, 1> & vec2)
{
	return vec1[0] * vec2[1] - vec1[1] * vec2[0];
}


// Input W must be a unit-length vector.  The output vectors {U,V} are
// unit length and mutually perpendicular, and {U,V,W} is an orthonormal
// basis.
template<typename T>
inline void generateComplementBasis(Eigen::Matrix<T, 3, 1> & u,
	Eigen::Matrix<T, 3, 1> & v,
	const Eigen::Matrix<T, 3, 1> & w)
{
	T invLength;

	if (Math<T>::FAbs(w[0]) >= Math<T>::FAbs(w[1]))
	{
		// W.x or W.z is the largest magnitude component, swap them
		invLength = Math<T>::InvSqrt(w[0] * w[0] + w[2] * w[2]);
		u[0] = -w[2] * invLength;
		u[1] = (T)0;
		u[2] = +w[0] * invLength;
		v[0] = w[1] * u[2];
		v[1] = w[2] * u[0] - w[0] * u[2];
		v[2] = -w[1] * u[0];
	}
	else
	{
		// W.y or W.z is the largest magnitude component, swap them
		invLength = Math<T>::InvSqrt(w[1] * w[1] + w[2] * w[2]);
		u[0] = (T)0;
		u[1] = +w[2] * invLength;
		u[2] = -w[1] * invLength;
		v[0] = w[1] * u[2] - w[2] * u[1];
		v[1] = -w[0] * u[2];
		v[2] = w[0] * u[1];
	}
}

template <typename T>
inline T normalizeVec2(Eigen::Matrix<T, 2, 1>& vec, const T epsilon = Math<T>::ZERO_TOLERANCE)
{
	T length = vec.norm();

	if (length > epsilon)
	{
		T invLength = ((T)1) / length;
		vec[0] *= invLength;
		vec[1] *= invLength;
	}
	else
	{
		length = (T)0;
		vec[0] = (T)0;
		vec[1] = (T)0;
	}

	return length;
}

template <typename T>
inline T normalizeVec3(Eigen::Matrix<T, 3, 1>& vec, const T epsilon = Math<T>::ZERO_TOLERANCE)
{
	T length = vec.norm();

	if (length > epsilon)
	{
		T invLength = ((T)1) / length;
		vec[0] *= invLength;
		vec[1] *= invLength;
		vec[2] *= invLength;
	}
	else
	{
		length = (T)0;
		vec[0] = (T)0;
		vec[1] = (T)0;
		vec[2] = (T)0;
	}

	return length;
}

template <typename T>
T angleInRadians(const Eigen::Matrix<T, 3, 1> & vec1, const Eigen::Matrix<T, 3, 1> & vec2)
{

	float dot = vec1.dot(vec2);
	Eigen::Matrix<T, 3, 1> cross = vec1.cross(vec2);

	return  atan2(cross.norm(), dot);
}

template <typename T>
inline bool isPointInTriangle3(const Eigen::Matrix<T, 3, 1> & t0, 
					    const Eigen::Matrix<T, 3, 1> & t1,
	                    const Eigen::Matrix<T, 3, 1> & t2,
	                    const Eigen::Matrix<T, 3, 1> & p, T tolerance = Math<T>::ZERO_TOLERANCE)
{
	// check distance to plane of triangle first
	Plane3<T> plane(t0, t1, t2);
	T dist = Math<T>::FAbs(plane.distanceTo(p));
	if (dist > tolerance)
	{
		LOG(DBUG) << "Incoming point is offset from the plane of the triangle: " << dist;
		return false;
	}

	Eigen::Matrix<T, 3, 1> u = t1 - t0;
	Eigen::Matrix<T, 3, 1> v = t2 - t0;
	Eigen::Matrix<T, 3, 1> n = u.cross(v);
	Eigen::Matrix<T, 3, 1> w = p - t0;

	T len = n.squaredNorm();
	if (len < Math<T>::ZERO_TOLERANCE) return false;

	// compute barycentric coords
	T a = (u.cross(w).dot(n)) / len;
	T b = (w.cross(v).dot(n)) / len;
	T c = 1 - a - b;
	
	return ((a >= 0 && a <= 1) && (b >= 0 && b <= 1) && (c >= 0 && c <= 1));
}

#define PRINT_TRANSLATION vecStr3f(pBody->t.worldTransform.translation(), DBUG, __FUNCTION_NAME__);

// from Polymer https://github.com/ddiakopoulos/polymer
///////////////////////////////////////////
   //   spherical & cartesian coordinates   //
   ///////////////////////////////////////////

   // These functions adopt the physics convention (ISO):
   // * (rho) r defined as the radial distance, 
   // * (theta) θ defined as the the polar angle (inclination)
   // * (phi) φ defined as the azimuthal angle (zenith)

   // These conversion routines assume the following: 
   // * the systems have the same origin
   // * the spherical reference plane is the cartesian xy-plane
   // * θ is inclination from the z direction
   // * φ is measured from the cartesian x-axis (so that the y-axis has φ = +90°)

   // theta ∈ [0, π], phi ∈ [0, 2π), rho ∈ [0, ∞)
inline Eigen::Vector3f cartsesian_coord(float thetaRad, float phiRad, float rhoRad = 1.f)
{
	return Eigen::Vector3f(rhoRad * sin(thetaRad) * cos(phiRad), rhoRad * sin(phiRad) * sin(thetaRad), rhoRad * cos(thetaRad));
}

inline Eigen::Vector3f spherical_coord(const Eigen::Vector3f & coord)
{
	const float radius = coord.norm();
	return Eigen::Vector3f(radius, std::acos(coord.z() / radius), std::atan(coord.y() / coord.x()));
}