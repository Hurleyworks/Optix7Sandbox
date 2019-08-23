// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks

#pragma once

#include <sabi_core/sabi_core.h>

using handleType = std::unique_ptr<uint8_t[], void(*)(void*)>;
using sabi::PrimitiveType;
using sabi::RenderableDesc;
using sabi::MeshOptions;
using sabi::MeshBuffersHandle;
using sabi::PixelBuffer;
using sabi::LoadStrategyHandle;
using sabi::RenderableNode;
using sabi::RenderableData;

class Model : public CsSignal::SlotBase
{
	//https://www.bogotobogo.com/DesignPatterns/mvc_model_view_controller_pattern.php
	// Manages the app data and state
	// Not concerned with UI or presentation
	// Often persists somewhere
	// Same model should be reusable, unchanged in different interfaces

 public:
	Model (const PropertyService& properties);
	~Model ();

	void loadPrimitive(PrimitiveType type, MeshOptions options = MeshOptions());
	void createGroundPlane(const Eigen::Vector2f& size);

	void loadImage(const std::string& path, PixelBuffer & buffer);
	void onDrop(const std::vector<std::string>& fileList);

	void createNewRenderable(const RenderableData& d);
	void createNewInstance(const RenderableData& d);

	void addNode(RenderableNode& node);
	void removeNode(RenderableNode& node);
	void removeNode(ItemID itemID);

	void clearScene();
	
 private:
	PropertyService properties;
	RenderableNode world = nullptr;
	LoadStrategyHandle loadStrategy = nullptr;

	void addMesh(MeshBuffersHandle mesh,
				 const std::string& name,
				 BodyID clientID,
				 const Pose& pose,
				 const Scale& scale,
				 const RenderableDesc& desc,
				 MeshOptions options);

	

}; // end class Model
