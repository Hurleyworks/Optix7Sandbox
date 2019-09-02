// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

using sabi::CameraHandle;

using RendererHandle = std::unique_ptr<class OptixRenderer>;

class OptixRenderer
{

 public:
	virtual ~OptixRenderer ();

	virtual void resize(unsigned int screenWidth, unsigned int screenHeight) = 0;
	virtual void render(CameraHandle& camera, OptixEngineRef& engine) = 0;

 protected:
	OptixRenderer(unsigned int screenWidth, unsigned int screenHeight);

	unsigned int width = DEFAULT_DESKTOP_WINDOW_WIDTH;
	unsigned int height = DEFAULT_DESKTOP_WINDOW_HEIGHT;

}; // end class OptixRenderer
