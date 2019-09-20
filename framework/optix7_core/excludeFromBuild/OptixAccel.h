// This header file was auto-generated by ClassMate++
// Created: 12 Sep 2019 6:29:23 am
// Copyright (c) 2019, HurleyWorks

#pragma once

using sabi::RenderableWeakRef;
using sabi::RenderableNode;
using sabi::MeshBuffersHandle;

class OptixAccel
{
	struct GASInfo
	{
		std::vector<OptixBuildInput> buildInputs;
		OptixAccelBufferSizes gas_buffer_sizes;
		MeshBuffersHandle mesh;
	};

 public:
	OptixAccel ();
	~OptixAccel ();
	
private:
	OptixAccelBuildOptions accel_options = {};

}; // end class OptixAccel
