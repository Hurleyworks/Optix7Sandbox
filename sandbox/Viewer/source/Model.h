// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks

#pragma once

#include <sabi_core/sabi_core.h>
#include <concurrent.hpp>

using handleType = std::unique_ptr<uint8_t[], void(*)(void*)>;
using namespace sabi;
using Eigen::Vector2f;

class Model : public CsSignal::SlotBase, public CsSignal::SignalBase
{
	//https://www.bogotobogo.com/DesignPatterns/mvc_model_view_controller_pattern.php
	// Manages the app data and state
	// Not concerned with UI or presentation
	// Often persists somewhere
	// Same model should be reusable, unchanged in different interfaces

public:
	using MeshData = std::pair<MeshBuffersHandle, std::string>;

	using ErrorQueue = std::queue<ErrMsg>;
	using MeshQueue = moodycamel::ConcurrentQueue<MeshData>;
	using ImageQueue = moodycamel::ConcurrentQueue<PixelBufferHandle>;

	SIGNAL_1(void emitRenderable(RenderableNode& node))
	SIGNAL_2(emitRenderable, node)

public:
	Model(const PropertyService& properties);
	~Model();

	ErrMsg getNextErrorMessage()
	{
		if (!errorQueue.empty())
		{
			ErrMsg error = errorQueue.front();
			errorQueue.pop();
			return error;
		}
		return ErrMsg();
	}

	MeshData getNextLoadedMeshBuffer()
	{
		MeshData data;
		bool found = meshQueue.try_dequeue(data);

		return found ? data : MeshData();
	}

	PixelBufferHandle getNextLoadedImage()
	{
		PixelBufferHandle image;
		bool found = imageQueue.try_dequeue(image);

		return found ? image : nullptr;
	}

	void addPrimitive(PrimitiveType type, MeshOptions options = MeshOptions());
	void createGroundPlane(const Vector2f& size = Vector2f(100.0f, 100.0f));
	void onMeshLoad(MeshBuffersHandle m, std::string path);
	void onImagesLoad(const Images&& images);
	void addMesh(MeshBuffersHandle mesh,
				const std::string& path,
				BodyID clientID,
				const Pose& pose,
				const Scale& scale,
				RenderableDesc desc,
				MeshOptions options);

	void loadImage(const std::string& path, PixelBuffer& buffer);
	void loadModelFromIcon(const std::string& iconPath);
	void onDrop(const std::vector<std::string>& fileList);

	void createNewRenderable(const RenderableData& d);
	void createNewInstance(const RenderableData& d);
	
	void addNode(RenderableNode& node);
	void removeNode(RenderableNode& node);
	void removeNode(ItemID itemID);

	void clearScene();

private:
	PropertyService properties;
	LoadMeshCallback loadMeshCallback = nullptr;
	LoadImagesCallback loadImagesCallback = nullptr;
	RenderableNode world = nullptr;
	LoadStrategyHandle loadStrategy = nullptr;

	ErrorQueue errorQueue;

	// thread safe queues
	MeshQueue meshQueue;
	ImageQueue imageQueue;

	// scene images
	Images sceneImages;

	// obj, ply, off loader via libIGL
	concurrent<ActiveLoader> activeLoader;

	// glTF 2.0
	concurrent<glTFLoader> glTFLoader;

}; // end class Model
