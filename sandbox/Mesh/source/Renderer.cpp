// This source file was auto-generated by ClassMate++
// Created: 31 Aug 2019 5:31:43 am
// Copyright (c) 2019, HurleyWorks

#include "Renderer.h"

using sabi::PixelBuffer;

// ctor
Renderer::Renderer (unsigned int screenWidth, unsigned int screenHeight)
	: OptixRenderer(screenWidth, screenHeight)
{	
	output_buffer.init(CUDAOutputBufferType::ZERO_COPY, screenWidth, screenHeight);

	params.image_width = width;
	params.image_height = height;
	params.origin_x = width / 2;
	params.origin_y = height / 2;

	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_param), sizeof(Params)));
	CUDA_CHECK(cudaStreamCreate(&stream));;
}

// dtor
Renderer::~Renderer ()
{	
}

void Renderer::resize(unsigned int screenWidth, unsigned int screenHeight)
{
	params.image_width = width;
	params.image_height = height;
	params.origin_x = width / 2;
	params.origin_y = height / 2;
}

void Renderer::render(CameraHandle& camera, OptixEngineRef& engine)
{
	params.image = output_buffer.map();
	params.handle = engine->getGAS();

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(d_param),
		&params, sizeof(params),
		cudaMemcpyHostToDevice
	));

	OPTIX_CHECK(optixLaunch(engine->getPipeline(), stream, d_param, sizeof(Params), engine->getSBT(), width, height, /*depth=*/1));
	CUDA_SYNC_CHECK();

	output_buffer.unmap();

	// capture the Optix render 
	PixelBuffer& pixels = camera->getPixelBuffer();
	std::memcpy(pixels.uint8Pixels.data(), output_buffer.getHostPointer(), pixels.byteCountUint8());
}

