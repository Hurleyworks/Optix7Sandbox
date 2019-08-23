// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

using sabi::CameraHandle;

class OptixRenderer
{

 public:
	OptixRenderer (const PropertyService& properties);
	~OptixRenderer ();

	void render(CameraHandle& camera);

 private:
	 PropertyService properties;

}; // end class OptixRenderer
