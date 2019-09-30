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
	RAY_TYPE_PICK,
	RAY_TYPE_COUNT,
};

struct PickingPayload
{
	uint32_t meshID;
	uint32_t primitiveID;
};

const uint32_t NUM_PAYLOAD_VALUES = 2u;

struct MissData
{
	float r, g, b;
};

struct HitGroupSBT
{
	OptixGeometryData geometry_data;
	OptixMaterialData material_data;
	
};

const std::string pickHitName = "PickHit";
const std::string pickMissName = "PickMiss";
const std::string pickGenName = "PickGen";
