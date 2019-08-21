// This source file was auto-generated by ClassMate++
// Created: 26 Jan 2019 8:47:47 pm
// Copyright (c) 2019, HurleyWorks

#include "berserkpch.h"
#include "App.h"

namespace Jahley
{
	
	App::App (DesktopWindowSettings settings, bool windowApp)
		: windowApp(windowApp),
		  errorCallback(std::bind(&App::onFatalError, this, std::placeholders::_1)),
		  preCrashCallback(std::bind(&App::preCrash, this)),
		  log(errorCallback, preCrashCallback),
		  refreshTime(settings.refreshRate)
	{	
		resetProperties();

		if (windowApp)
		{
			try
			{
				bgColor = properties.renderProps->getVal<Eigen::Vector4f>(RenderKey::BackgroundColor);
				window = std::make_unique<OpenglWindow>(input);
				window->create(Vector2i(settings.width, settings.height), settings.name, settings.resizable);

				// broadcast InputHandler events to the render layers
				connect(input, &InputHandler::onEvent, *this, &App::onInputEvent);

				// send drop to client
				connect(input, &InputHandler::onDragAndDrop, *this, &App::onDrop);
			}
			catch (std::exception& e)
			{
				LOG(CRITICAL) << e.what();
			}
			catch (...)
			{
				LOG(CRITICAL) << "Caught unknown exception!";
			}
		}
	}

	void App::resetProperties()
	{
		properties.init();

		// world properties
		properties.worldProps->addDefault(WorldKey::TotalMeshes, DEFAULT_MESH_COUNT);
		properties.worldProps->addDefault(WorldKey::TotalInstances, DEFAULT_INSTANCES_COUNT);
		properties.worldProps->addDefault(WorldKey::TotalRealTriangles, DEFAULT_TRIANGLE_COUNT);
		properties.worldProps->addDefault(WorldKey::TotalInstancedTriangles, DEFAULT_INSTANCED_TRIANGLE_COUNT);

		// render properties
		properties.renderProps->addDefault(RenderKey::BackgroundColor, DEFAULT_DESKTOP_WINDOW_BACKGROUND_COLOR);
		properties.renderProps->addDefault(RenderKey::ResourceFolder, INVALID_PATH);
		properties.renderProps->addDefault(RenderKey::CommonFolder, INVALID_PATH);
		properties.renderProps->addDefault(RenderKey::ShowPerformanceGraph, true);
	}

	App::~App ()
	{	
	
	}

	void App::run()
	{
		// let the client initialize
		onInit();

		while (input.windowIsOpen())
		{
			window->renderBegin(properties.renderProps->getVal<Vector4f>(RenderKey::BackgroundColor));

			for (RenderLayerRef layer : layers)
				layer->onUpdate();

			// let the client update
			update();

			window->renderEnd(true); // true means glfw will wait for events instead of polling

			// post an empty event to GLFW
			std::this_thread::sleep_for(refreshTime);
			glfwPostEmptyEvent();
		}

		window.reset();
	}

	void App::pushLayer(RenderLayerRef layer, bool attach)
	{
		layers.pushLayer(layer);
		if (attach)
			layer->onAttach();
	}

	void App::pushOverlay(RenderLayerRef layer, bool attach)
	{
		layers.pushOverlay(layer);
		if (attach)
			layer->onAttach();
	}

	void App::onInputEvent(const InputEvent & e)
	{
		for (RenderLayerRef layer : layers)
			layer->onInput(e);

		// send to client
		onInput(e);
	}

	// preCrash
	void App::preCrash()
	{
		// let the client throw up a warning if it can
		onCrash();

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
		__debugbreak();
#endif
	}

	// onFatalError
	void App::onFatalError(g3::FatalMessagePtr fatal_message)
	{
		LOG(CRITICAL) << fatal_message.get()->toString();

		g3::internal::pushFatalMessageToLogger(fatal_message);
	}
}