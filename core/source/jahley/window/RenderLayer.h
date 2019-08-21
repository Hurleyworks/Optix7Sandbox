// This header file was auto-generated by ClassMate++
// Created: 17 Aug 2019 3:42:21 pm
// Copyright (c) 2019, HurleyWorks

#pragma once
#include <sabi_core/sabi_core.h>

// https://www.youtube.com/user/TheChernoProject

using RenderLayerRef = std::shared_ptr<class RenderLayer>;
using sabi::CameraHandle;
using sabi::InputEvent;

class InputEvent;

class RenderLayer
{

 public:
	RenderLayer (const PropertyService& properties, CameraHandle  camera = nullptr);
	~RenderLayer ();

	virtual void onUpdate() = 0;
	virtual void onInput(const InputEvent& input) = 0;

	virtual void onAttach() {}
	virtual void onDetach() {}

	virtual void postInfoMessage(const std::string& title, const std::string& msg) {}
	virtual void postWarningMessage(const std::string& title, const std::string& msg) {}
	virtual void postQuestiongMessage(const std::string& title, const std::string& msg) {}

	CameraHandle cam() { return camera; }

 protected:
	PropertyService properties;
	CameraHandle camera = nullptr;

}; // end class RenderLayer
