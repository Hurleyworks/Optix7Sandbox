// ======================================================================== //
// Copyright 2018-2019 Ingo Wald                                            //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "ShaderRecordData.h"
#include <stdint.h>

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
	float4* accum_buffer;
	uchar4* frame_buffer;

	uint32_t subframe_index;
	
	int32_t max_depth;
	float gamma;
	uint32_t               image_width;
	uint32_t               image_height;
	int32_t                origin_x;
	int32_t                origin_y;
	
	OptixBufferView<OptixLight::Point> lights;

};



// program group names
const std::string radianceMissName = "radiance_miss_group";
const std::string occlusionMissName = "occlusion_miss_group";

const std::string radianceHitName = "hit_group_radiance";
const std::string occlusionHitName = "hit_group_occlusion";

const std::string raygentName = "raygen_group";
