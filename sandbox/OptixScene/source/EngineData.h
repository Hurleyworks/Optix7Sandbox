#include <optix7_core/optix7_core.h>

template <typename T>
struct Record
{
	__align__(OPTIX_SBT_RECORD_ALIGNMENT) char header[OPTIX_SBT_RECORD_HEADER_SIZE];
	T data;
};

struct EmptyData {};

typedef Record<EmptyData> EmptyRecord;

namespace whitted
{

	const uint32_t NUM_PAYLOAD_VALUES = 4u;


	struct HitGroupData
	{
		GeometryData geometry_data;
		MaterialData material_data;
	};


	enum RayType
	{
		RAY_TYPE_RADIANCE = 0,
		RAY_TYPE_OCCLUSION = 1,
		RAY_TYPE_COUNT = 2
	};


	struct LaunchParams
	{
		uint32_t                 subframe_index;
		float4* accum_buffer;
		uchar4* frame_buffer;
		int32_t                  max_depth;

		float3                   eye;
		float3                   U;
		float3                   V;
		float3                   W;

		BufferView<Light::Point> lights;
		float3                   miss_color;
		OptixTraversableHandle   handle;
	};


	struct PayloadRadiance
	{
		float3 result;
		float  importance;
		int    depth;
	};


	struct PayloadOcclusion
	{
	};


} // end namespace whitted


