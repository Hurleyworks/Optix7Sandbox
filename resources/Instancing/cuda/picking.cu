#include <optix.h>
#include "OptixPreprocessor.h"
#include "OptixMath.h"
#include "OptixBufferView.h"
#include "OptixLight.h"
#include "OptixGeometry.h"
#include "OptixMaterialData.h"
#include "OptixRecordData.h"
#include "OptixLaunchParams.h"
#include <string>

extern "C" {
__constant__ PickParams params;
}

static __forceinline__ __device__ void pick(
        OptixTraversableHandle      handle,
        float3                      ray_origin,
        float3                      ray_direction,
        float                       tmin,
        float                       tmax,
        PickingPayload*   payload
        )
{
    uint32_t u0=0, u1=0;
    optixTrace(
            handle,
            ray_origin, ray_direction,
            tmin,
            tmax,
            0.0f, // rayTime
            OptixVisibilityMask( 1 ),
            OPTIX_RAY_FLAG_NONE,
            PICK_RAY_TYPE_PICK,     // SBT offset
            PICK_RAY_TYPE_COUNT,    // SBT stride
            PICK_RAY_TYPE_PICK,     // missSBTIndex
            u0, u1);

	payload->meshID = u0;
	payload->primitiveID = u1;
}

extern "C" __global__ void __raygen__pickRay()
{
	if(!params.picking)	{return;}

	// trace pick ray
    PickingPayload payload;
	payload.meshID = ~0;
	payload.primitiveID = ~0;
   
	pick(
            params.sceneAccel,
            params.rayOrigin,
            params.rayDir,
            0.01f,  // tmin       // TODO: smarter offset
            1e16f,  // tmax
            &payload );
			
	unsigned int* pickData = reinterpret_cast<unsigned int*>(params.pickBuffer);

	pickData[0] = payload.meshID;
	pickData[1] = payload.primitiveID;
  
	//printf("Picked mesh: %i\n", pickData[0] ); 
	// printf("Picked triangle:  %i\n", pickData[1]); 
}

extern "C" __global__ void __closesthit__pickHit()
{
	 unsigned int primID = optixGetPrimitiveIndex();
	 unsigned int instID = optixGetInstanceId();
	 
	 optixSetPayload_0( static_cast<unsigned int>( instID ) );
	 optixSetPayload_1( static_cast<unsigned int>( primID ) );
}

extern "C" __global__ void __miss__pickMiss()
{
	optixSetPayload_0( static_cast<unsigned int>( ~0 ) );
	optixSetPayload_1( static_cast<unsigned int>( ~0 ) );
   
}
	