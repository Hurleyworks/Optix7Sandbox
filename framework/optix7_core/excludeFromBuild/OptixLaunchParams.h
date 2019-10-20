// This header file was auto-generated by ClassMate++
// Created: 26 Sep 2019 2:36:23 pm
// Copyright (c) 2019, HurleyWorks

#pragma  once

// https://devtalk.nvidia.com/default/topic/1063868/optix/pick-ray-in-optix7/
// The OptixTraversableHandle is a 64 - bit value and must lie on an 8 - byte aligned address.Same for CUdeviceptr.
// All other fields in that PickParams stucture require only a 4 - byte aligned addresses.
// I recommend to always order all device side structures by alignment requirements with the largest first.
// If the structure is used in an array, I even manually pad its overall size to the largest alignment.

// I would make the picked IDs unsigned int because both optixGetPrimitiveIndex() and optixGetInstanceId()
// return unsigned int values and the payload registers are also unsigned int references, which means there is no need for any integer conversions.
// You can use ~0 as miss identifier which is safe in OptiX 7.0.0 because neither
// OPTIX_DEVICE_PROPERTY_LIMIT_MAX_PRIMITIVES_PER_GAS nor OPTIX_DEVICE_PROPERTY_LIMIT_MAX_INSTANCES_PER_IAS can exceed 4 Gig.

struct PickParams
{
	// 8-byte aligned.
	OptixTraversableHandle sceneAccel;
	CUdeviceptr pickBuffer; // CUDA device pointer with room for at least two unsigned int.

	// 4-byte aligned
	int picking;
	float3 rayOrigin;
	float3 rayDir;
};

struct WhittedParams
{
	OptixTraversableHandle sceneAccel;
	uint32_t subframe_index;
	float4* accum_buffer;
	uchar4* frame_buffer;
	int32_t max_depth;
	float gamma;
	uint32_t               image_width;
	uint32_t               image_height;
	int32_t                origin_x;
	int32_t                origin_y;
	
	OptixBufferView<OptixLight::Point> lights;

};


struct SystemData
{
	// https://devtalk.nvidia.com/default/topic/1063868/optix/pick-ray-in-optix7/post/5391297/#5391297

	// 8-byte alignment
	OptixTraversableHandle topObject;

	// The accumulated linear color space output buffer.
	// This is always sized to the resolution, not always matching the launch dimension.
	// Using a CUdeviceptr here to allow for different buffer formats without too many casts.
	CUdeviceptr outputBuffer;
	float4* accum_buffer;
	int2 resolution;   // The resolution of the full image in outputBuffer. Independent from the launch dimensions for some rendering strategies.

	CUdeviceptr pickingBuffer; // Buffer which will receive all information of the primary hit when pickingEnabled != 0.
	float2 pickingFragment;  // Pixel center coordinate on the full resolution image to shoot a primary picking ray for with the current projection.

	// 4 byte alignment 
	int pickingEnabled; // Flag indicating that the pickingPixel should be used to calculate the primary ray. Launch at 1x1 size in that case.
	uint32_t subframe_index;

	OptixBufferView<OptixLight::Point> lights; // alignment???
};