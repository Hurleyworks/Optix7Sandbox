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

#include <optix7_core/optix7_core.h>

struct Params
{
	uchar4* image;
	uint32_t               image_width;
	uint32_t               image_height;
	int32_t                origin_x;
	int32_t                origin_y;
	OptixTraversableHandle handle;
};


struct RayGenData
{
	float3 cam_eye;
	float3 camera_u, camera_v, camera_w;
};


struct MissData
{
	float r, g, b;
};

struct HitData
{
	float r, g, b;
};


