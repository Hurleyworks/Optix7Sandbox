#pragma once

#ifdef OPTIX_APP
#include "optix7_core/optix7_core.h"
#else
#include <optix.h>
#include "OptixPreprocessor.h"
#include "OptixMath.h"
#include "OptixBufferView.h"
#include "OptixLight.h"
#include "OptixGeometryData.h"
#include "OptixMaterialData.h"
#include <string>
#endif

struct RayGenData
{
	float3 cam_eye;
	float3 camera_u, camera_v, camera_w;
};


enum RayType
{
	RAY_TYPE_RADIANCE,
	RAY_TYPE_OCCLUSION,
	RAY_TYPE_COUNT,
};

struct PayloadRadiance
{
	float3 result;
	float  importance;
	int    depth;
};

const uint32_t NUM_PAYLOAD_VALUES = 4u;

struct MissData
{
	float r, g, b;
};

struct HitGroupSBT
{
	OptixGeometryData geometry_data;
	OptixMaterialData material_data;
};

// program group names
const std::string radianceMissName = "radiance_miss_group";
const std::string occlusionMissName = "occlusion_miss_group";

const std::string radianceHitName = "radiance_hit_group";
const std::string occlusionHitName = "occulsion_hit_group";

const std::string raygentName = "raygen_group";