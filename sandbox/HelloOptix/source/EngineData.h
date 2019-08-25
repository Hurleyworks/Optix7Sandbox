#include <optix7_core/optix7_core.h>

using juce::String;

struct RayGenData
{
	float3 cam_eye;
	float3 camera_u, camera_v, camera_w;
};


struct MissData
{
	float r, g, b;
};


struct HitGroupData
{
	float radius;
};

template <typename T>
struct SbtRecord
{
	__align__(OPTIX_SBT_RECORD_ALIGNMENT) char header[OPTIX_SBT_RECORD_HEADER_SIZE];
	T data;
};

typedef SbtRecord<RayGenData>     RayGenSbtRecord;
typedef SbtRecord<MissData>       MissSbtRecord;
typedef SbtRecord<HitGroupData>   HitGroupSbtRecord;

