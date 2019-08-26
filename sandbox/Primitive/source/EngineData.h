#include <optix7_core/optix7_core.h>

using juce::String;

enum RayType
{
	RAY_TYPE_RADIANCE = 0,
	RAY_TYPE_OCCLUSION = 1,
	RAY_TYPE_COUNT
};


struct BasicLight
{
	float3  pos;
	float3  color;
};


struct WhittedParams
{
	uint32_t     subframe_index;
	float4* accum_buffer;
	uchar4* frame_buffer;
	uint32_t     width;
	uint32_t     height;

	BasicLight   light;                 // TODO: make light list
	float3       ambient_light_color;
	int          max_depth;
	float        scene_epsilon;

	OptixTraversableHandle  handle;
};


struct CameraData
{
	float3       eye;
	float3       U;
	float3       V;
	float3       W;
};


struct MissData
{
	float3 bg_color;
};


struct Sphere
{
	float3	center;
	float 	radius;
};


enum SphereShellHitType {
	HIT_OUTSIDE_FROM_OUTSIDE = 1u << 0,
	HIT_OUTSIDE_FROM_INSIDE = 1u << 1,
	HIT_INSIDE_FROM_OUTSIDE = 1u << 2,
	HIT_INSIDE_FROM_INSIDE = 1u << 3
};


struct SphereShell
{
	float3 	center;
	float 	radius1;
	float 	radius2;
};


struct Parallelogram
{
	Parallelogram() = default;
	Parallelogram(float3 v1, float3 v2, float3 anchor) :
		v1(v1), v2(v2), anchor(anchor)
	{
		float3 normal = normalize(cross(v1, v2));
		float d = dot(normal, anchor);
		this->v1 *= 1.0f / dot(v1, v1);
		this->v2 *= 1.0f / dot(v2, v2);
		plane = make_float4(normal, d);
	}
	float4	plane;
	float3 	v1;
	float3 	v2;
	float3 	anchor;
};


struct Phong
{
	float3 Ka;
	float3 Kd;
	float3 Ks;
	float3 Kr;
	float  phong_exp;
};


struct Glass
{
	float  importance_cutoff;
	float3 cutoff_color;
	float  fresnel_exponent;
	float  fresnel_minimum;
	float  fresnel_maximum;
	float  refraction_index;
	float3 refraction_color;
	float3 reflection_color;
	float3 extinction_constant;
	float3 shadow_attenuation;
	int    refraction_maxdepth;
	int    reflection_maxdepth;
};


struct CheckerPhong
{
	float3 Kd1, Kd2;
	float3 Ka1, Ka2;
	float3 Ks1, Ks2;
	float3 Kr1, Kr2;
	float  phong_exp1, phong_exp2;
	float2 inv_checker_size;
};


struct HitGroupData
{
	union
	{
		Sphere          sphere;
		SphereShell     sphere_shell;
		Parallelogram   parallelogram;
	} geometry;

	union
	{
		Phong           metal;
		Glass           glass;
		CheckerPhong    checker;
	} shading;
};


struct RadiancePRD
{
	float3 result;
	float  importance;
	int    depth;
};


struct OcclusionPRD
{
	float3 attenuation;
};

//------------------------------------------------------------------------------

template <typename T>
struct Record
{
	__align__(OPTIX_SBT_RECORD_ALIGNMENT)

		char header[OPTIX_SBT_RECORD_HEADER_SIZE];
	T data;
};

typedef Record<CameraData>      RayGenRecord;
typedef Record<MissData>        MissRecord;
typedef Record<HitGroupData>    HitGroupRecord;

const uint32_t OBJ_COUNT = 3;

struct WhittedState
{
	OptixDeviceContext          context = 0;
	OptixTraversableHandle      gas_handle = {};
	CUdeviceptr                 d_gas_output_buffer = {};

	OptixModule                 geometry_module = 0;
	OptixModule                 camera_module = 0;
	OptixModule                 shading_module = 0;

	OptixProgramGroup           raygen_prog_group = 0;
	OptixProgramGroup           radiance_miss_prog_group = 0;
	OptixProgramGroup           occlusion_miss_prog_group = 0;
	OptixProgramGroup           radiance_glass_sphere_prog_group = 0;
	OptixProgramGroup           occlusion_glass_sphere_prog_group = 0;
	OptixProgramGroup           radiance_metal_sphere_prog_group = 0;
	OptixProgramGroup           occlusion_metal_sphere_prog_group = 0;
	OptixProgramGroup           radiance_floor_prog_group = 0;
	OptixProgramGroup           occlusion_floor_prog_group = 0;

	OptixPipeline               pipeline = 0;
	OptixPipelineCompileOptions pipeline_compile_options = {};

	CUstream                    stream = 0;
	WhittedParams                      params;
	WhittedParams* d_params = nullptr;

	OptixShaderBindingTable     sbt = {};
};

//------------------------------------------------------------------------------
//
//  Geometry and Camera data
//
//------------------------------------------------------------------------------

// Metal sphere, glass sphere, floor, light
const Sphere g_sphere = {
	{ 2.0f, 1.5f, -2.5f }, // center
	1.0f                   // radius
};
const SphereShell g_sphere_shell = {
	{ 4.0f, 2.3f, -4.0f }, // center
	0.96f,                 // radius1
	1.0f                   // radius2
};
const Parallelogram g_floor(
	make_float3(32.0f, 0.0f, 0.0f),    // v1
	make_float3(0.0f, 0.0f, 16.0f),    // v2
	make_float3(-16.0f, 0.01f, -8.0f)  // anchor
);
const BasicLight g_light = {
	make_float3(60.0f, 40.0f, 0.0f),   // pos
	make_float3(1.0f, 1.0f, 1.0f)      // color
};
