// This header file was auto-generated by ClassMate++
// Created: 22 Feb 2019 8:55:26 pm
// Copyright (c) 2019, HurleyWorks

#pragma  once

// NO MORE TRACING  Yay
#define TRACE(x)

const LEVELS TESTING{ INFO.value + 1, "TESTING" };
const LEVELS CRITICAL{ WARNING.value + 1, "CRTICAL" };

#ifndef __FUNCTION_NAME__
#if defined(JUCE_WINDOWS)
#define __FUNCTION_NAME__   __FUNCTION__  
#define _FN_  __FUNCTION_NAME__
#elif defined(JUCE_MAC)
#define __FUNCTION_NAME__   __func__ 
#define _FN_   __func__ 
#endif
#endif

#define BIT(x) (1 << x)


// default window settings
const float PHI = 1.618f;
const float DEFAULT_DESKTOP_WINDOW_HEIGHT = 720.0f;
const float DEFAULT_DESKTOP_WINDOW_WIDTH = DEFAULT_DESKTOP_WINDOW_HEIGHT * PHI;
const int DEFAULT_DESKTOP_WINDOW_CHANNELS = 3;
const std::string DEFAULT_DESKTOP_WINDOW_NAME = "DesktopWindow";
const int DEFAULT_DESKTOP_WINDOW_REFRESH_RATE = 16;
const bool DEFAULT_DESKTOP_WINDOW_RESIZABLE = true;
const float DEFAULT_ZOOM_FACTOR = 0.05f;
const Eigen::Vector4f DEFAULT_DESKTOP_WINDOW_BACKGROUND_COLOR = Eigen::Vector4f(0.2f, 0.22f, 0.24f, 1.0f);
const Eigen::Vector4f DEFAULT_MESH_COLOR = Eigen::Vector4f(1.0f, 0.5f, 0.0f, 1.0f);
const float DEFAULT_RENDER_GAMMA = 1.0f;

using ItemID = int64_t;
const int64_t INVALID_ID = -1;
const int64_t INVALID_INDEX = -1;
const std::string INVALID_PATH = "invalid path";
const std::string INVALID_NAME = "invalid name";
const std::string DEFAULT_ERROR_MESSAGE = "AOK";
const std::string DEFAULT_CAMERA_NAME = "Default_Camera";

using BodyID = int64_t;
using PolyID = int64_t;
using RenderedPixels = std::vector<uint8_t>;
using RenderedFloatPixels = std::vector<float>;
using ImagePixels = std::vector<uint8_t>;
using ImageFloatPixels = std::vector<float>;
typedef float Float;

const int DEFAULT_MIN_WINDOW = 10;
const int DEFAULT_CHANNELS = 3;

struct ImageInfo
{
	int width = DEFAULT_MIN_WINDOW;
	int height = DEFAULT_MIN_WINDOW;
	int channels = DEFAULT_CHANNELS;
};


typedef Eigen::Matrix<uint16_t, Eigen::Dynamic, Eigen::Dynamic> MatrixXus;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> MatrixXu;
typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>      MatrixXi;
typedef Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic>  MatrixXc;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, 1>              VectorXu;
typedef Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic>    MatrixXf;
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>   MatrixXd;
typedef Eigen::Matrix<bool, Eigen::Dynamic, 1>                  VectorXb;
typedef Eigen::Matrix<Float, 3, 3>                              Matrix3f;
typedef Eigen::Matrix<Float, 4, 3>                              Matrix43f;
typedef Eigen::Matrix<Float, 3, 4>                              Matrix34f;
typedef Eigen::Matrix<uint32_t, 1, 3>                           Vector3u;
typedef Eigen::Matrix<uint8_t, 1, 3>                            Vector3c;
typedef Eigen::Matrix<uint32_t, 1, 4>                           Vector4u;
typedef Eigen::Matrix<Float, Eigen::Dynamic, 1>                 VectorXf;

// from Rohan Sawhney https://github.com/rohan-sawhney?tab=repositories
template <typename T>
inline void hashCombiner(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// from Rohan Sawhney https://github.com/rohan-sawhney?tab=repositories
template <typename T>
inline size_t hashVec3(const Eigen::Matrix<T, 3, 1> & v)
{
	size_t hash = 0;
	hashCombiner(hash, v[0]);
	hashCombiner(hash, v[1]);
	hashCombiner(hash, v[2]);

	return hash;
}

template <typename Matrix> inline size_t sizeInBytes(const Matrix& matrix) {
	return matrix.size() * sizeof(typename Matrix::Scalar);
}

/// Always-positive modulo function, Float precision version (assumes b > 0)
inline Float modulo(Float a, Float b) {
	Float r = std::fmod(a, b);
	return (r < 0.0) ? r + b : r;
}

/// Always-positive modulo function (assumes b > 0)
inline int32_t modulo(int32_t a, int32_t b) {
	int32_t r = a % b;
	return (r < 0) ? r + b : r;
}

inline Float str_to_float(const std::string& str) {
	char* end_ptr = nullptr;
	Float result = (Float)strtod(str.c_str(), &end_ptr);
	if (*end_ptr != '\0')
		throw std::runtime_error("Could not parse floating point value \"" + str + "\"");
	return result;
}

//Instant Field - Aligned Meshes
//Wenzel Jakob, Daniele Panozzo, Marco Tarini, and Olga Sorkine - Hornung
//In ACM Transactions on Graphics(Proc.SIGGRAPH Asia 2015)
//
//All rights reserved.Use of this source code is governed by a
//BSD - style license that can be found in the LICENSE.txt file.

typedef std::function<void(const std::string&, Float)> ProgressCallback;

#define PROGRESS_BLKSIZE (1 << 18)
#define SHOW_PROGRESS(i, maxval, text) \
    if (progress && (i % PROGRESS_BLKSIZE) == 0) \
        progress(text, -PROGRESS_BLKSIZE / (Float) maxval)

#define PROGRESS_SHIFT 18u
#define SHOW_PROGRESS_RANGE(range, maxval, text) \
    if (progress && range.begin() > 0) { \
        uint32_t nUpdates = (range.end() >> PROGRESS_SHIFT) - ((range.begin() - 1) >> PROGRESS_SHIFT); \
        if (nUpdates > 0) { \
            const uint32_t nUpdatesTotal = (uint32_t) (maxval) / (1 << PROGRESS_SHIFT); \
            progress(text, - (float) nUpdates / (float) nUpdatesTotal); \
        } \
    }


#if defined(_WIN32)
#define RCPOVERFLOW_FLT   2.93873587705571876e-39f
#define RCPOVERFLOW_DBL   5.56268464626800345e-309
#else
#define RCPOVERFLOW_FLT   0x1p-128f
#define RCPOVERFLOW_DBL   0x1p-1024
#endif

#if defined(SINGLE_PRECISION)
#define RCPOVERFLOW RCPOVERFLOW_FLT
#else
#define RCPOVERFLOW RCPOVERFLOW_DBL
#endif

inline bool atomicCompareAndExchange(volatile uint32_t* v, uint32_t newValue, uint32_t oldValue) {
#if defined(_WIN32)
	return _InterlockedCompareExchange(
		reinterpret_cast<volatile long*>(v), (long)newValue, (long)oldValue) == (long)oldValue;
#else
	return __sync_bool_compare_and_swap(v, oldValue, newValue);
#endif
}

inline uint32_t atomicAdd(volatile uint32_t* dst, uint32_t delta) {
#if defined(_MSC_VER)
	return _InterlockedExchangeAdd(reinterpret_cast<volatile long*>(dst), delta) + delta;
#else
	return __sync_add_and_fetch(dst, delta);
#endif
}

inline float atomicAdd(volatile float* dst, float delta) {
	union bits { float f; uint32_t i; };
	bits oldVal, newVal;
	do {
#if defined(__i386__) || defined(__amd64__)
		__asm__ __volatile__("pause\n");
#endif
		oldVal.f = *dst;
		newVal.f = oldVal.f + delta;
	} while (!atomicCompareAndExchange((volatile uint32_t*)dst, newVal.i, oldVal.i));
	return newVal.f;
}

class ordered_lock
{
public:
	ordered_lock() : next_ticket(0), counter(0) {}
	void lock() {
		std::unique_lock<std::mutex> acquire(cvar_lock);
		unsigned int ticket = next_ticket++;
		while (ticket != counter)
			cvar.wait(acquire);
	}
	void unlock() {
		std::unique_lock<std::mutex> acquire(cvar_lock);
		counter++;
		cvar.notify_all();
	}
protected:
	std::condition_variable  cvar;
	std::mutex               cvar_lock;
	unsigned int             next_ticket, counter;
};


inline std::string timeString(double time, bool precise = false) {
	if (std::isnan(time) || std::isinf(time))
		return "inf";

	std::string suffix = "ms";
	if (time > 1000) {
		time /= 1000; suffix = "s";
		if (time > 60) {
			time /= 60; suffix = "m";
			if (time > 60) {
				time /= 60; suffix = "h";
				if (time > 12) {
					time /= 12; suffix = "d";
				}
			}
		}
	}

	std::ostringstream os;
	os << std::setprecision(precise ? 4 : 1)
		<< std::fixed << time << suffix;

	return os.str();
}

inline std::string memString(size_t size, bool precise = false) {
	double value = (double)size;
	const char* suffixes[] = {
		"B", "KiB", "MiB", "GiB", "TiB", "PiB"
	};
	int suffix = 0;
	while (suffix < 5 && value > 1024.0f) {
		value /= 1024.0f; ++suffix;
	}

	std::ostringstream os;
	os << std::setprecision(suffix == 0 ? 0 : (precise ? 4 : 1))
		<< std::fixed << value << " " << suffixes[suffix];

	return os.str();
}

inline std::string str_tolower(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

inline uint32_t str_to_uint32_t(const std::string& str) {
	char* end_ptr = nullptr;
	uint32_t result = (uint32_t)strtoul(str.c_str(), &end_ptr, 10);
	if (*end_ptr != '\0')
		throw std::runtime_error("Could not parse unsigned integer \"" + str + "\"");
	return result;
}

inline uint32_t str_to_int32_t(const std::string& str) {
	char* end_ptr = nullptr;
	int32_t result = (int32_t)strtol(str.c_str(), &end_ptr, 10);
	if (*end_ptr != '\0')
		throw std::runtime_error("Could not parse signed integer \"" + str + "\"");
	return result;
}

inline std::vector<std::string>& str_tokenize(const std::string& s, char delim, std::vector<std::string>& elems, bool include_empty = false) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
		if (!item.empty() || include_empty)
			elems.push_back(item);
	return elems;
}

inline std::vector<std::string> str_tokenize(const std::string& s, char delim, bool include_empty) {
	std::vector<std::string> elems;
	str_tokenize(s, delim, elems, include_empty);
	return elems;
}

inline void vecStr3f(const Eigen::Vector3f& v, LEVELS level, const char function[] = "unknown function")
{
	LOG(level) << "--- called from " << function;

	std::ostringstream buffer;
	buffer.setf(std::ios::fixed, std::ios::floatfield);
	buffer.precision(6);
	buffer << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";

	LOG(level) << buffer.str();
}

inline void vecStr3d(const Eigen::Vector3d& v, LEVELS level, const char function[] = "unknown function")
{
	LOG(level) << "--- called from " << function;

	std::ostringstream buffer;
	buffer.setf(std::ios::fixed, std::ios::floatfield);
	buffer.precision(8);
	buffer << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";

	LOG(level) << buffer.str();
}

inline void vecStr4f(const Eigen::Vector4f& v, LEVELS level, const char function[] = "unknown function")
{
	std::ostringstream buffer;
	buffer.setf(std::ios::fixed, std::ios::floatfield);
	buffer.precision(6);
	buffer << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";

	LOG(level) << buffer.str();
}

inline void matStr4f(const Eigen::Affine3f& t, LEVELS level, const char function[] = "unknown function")
{
	LOG(level) << "--- called from " << function;

	for (int i = 0; i < 4; i++)
		vecStr4f(t.matrix().row(i), level, function);
}

inline void matStr4f(const Eigen::Matrix4f& m, LEVELS level, const char function[] = "unknown function")
{
	LOG(level) << "--- called from " << function;

	for (int i = 0; i < 4; i++)
		vecStr4f(m.row(i), level, function);
}

inline void matStr3f(const Eigen::Matrix3f& m, LEVELS level, const char function[] = "unknown function")
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


// rational.cpp by Bill Weinman <http://bw.org/>
// updated 2015-06-01
inline void message(const char * s)
{
	puts(s);
	fflush(stdout);
}

enum ErrorSeverity {

	Information,
	Warning,
	Critical,
};

struct ErrMsg
{
	std::string message = "";
	ErrorSeverity severity = ErrorSeverity::Information;
};

// makes it illegal to copy a derived class
// https://github.com/heisters/libnodes 
struct Noncopyable
{
protected:
	Noncopyable() = default;
	~Noncopyable() = default;
	Noncopyable(const Noncopyable &) = delete;
	Noncopyable &operator=(const Noncopyable &) = delete;
};

// provides derived classes with automatically assigned,
// globally unique numeric identifiers
// https://github.com/heisters/libnodes
class HasId
{
public:
	HasId() : mId(++sId) {}

	ItemID id() const { return mId; }
	void setID(ItemID itemID) { mId = itemID; }

	void staticReset(int id = 0) { sId = id; }

protected:
	static ItemID sId;
	ItemID mId;
};

// from VLR
template <typename T, size_t size>
constexpr size_t lengthof(const T(&array)[size])
{
	return size;
}

