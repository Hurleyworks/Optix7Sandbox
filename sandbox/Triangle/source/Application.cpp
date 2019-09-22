#include "Jahley.h"

#include "OptixScene.h"
#include "OptixLayer.h"

#include "SceneConfig.h"
#include "Model.h"
#include "View.h"
#include "Controller.h"

const std::string APP_NAME = "Triangle";

using sabi::PixelBuffer;
using sabi::PerspectiveCam;
using Eigen::Vector3f;

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings, bool windowApp = false)
		: Jahley::App(settings, windowApp),
		  config(properties),
		  model(properties),
		  view(properties)
	{
		// store the resource folder for this project
		properties.renderProps->setValue(RenderKey::ResourceFolder, getResourcePath(APP_NAME).toStdString());

		// store the resource folder shared by all projects
		properties.renderProps->setValue(RenderKey::CommonFolder, getResourcePath("Common").toStdString());

		// create a camera
		camera = PerspectiveCam::create();
		float aspect = (float)settings.width / (float)settings.height;
		camera->setPerspective(DEFAULT_FOV_DEGREES, aspect, 1, 1000);
		camera->lookAt(Vector3f(0.0f, 0.0f, 2.0f), Eigen::Vector3f::Zero());

		// setup the camera's PixelBuffer
		ImageInfo spec;
		spec.width = DEFAULT_DESKTOP_WINDOW_WIDTH;
		spec.height = DEFAULT_DESKTOP_WINDOW_HEIGHT;
		spec.channels = 4;

		PixelBuffer& buffer = camera->getPixelBuffer();
		buffer.init(spec);
	}

	void onInit() override
	{
		try
		{
			config.init();

			// create the Gui overlay
			NanoguiLayer* const gui = new NanoguiLayer(window->glfw(), properties, camera);
			view.create(gui, APP_NAME);
			nanoguiLayer = RenderLayerRef(gui);
			pushOverlay(nanoguiLayer, true);

			// place this call after creating the gui so that 
			// we can display any error messages to the user
			// while creating an OptixEngine for this project
			createEngine();

			// create the Optix layer
			optixLayer = std::make_shared<OptixLayer>(properties, camera, engine);
			pushLayer(optixLayer, true);

			// connect the View with Model using signal/slots
			connect(view, &View::emitScreenGrab, *this, &Application::onScreenGrab);
		}
		catch (std::exception& e)
		{
			if (nanoguiLayer)
				nanoguiLayer->postWarningMessage("Fatal Error", e.what());

			LOG(CRITICAL) << e.what();
		}
	}

	void update() override
	{
		if (captureScreen)
			App::saveScreen(APP_NAME);

		checkForErrors();

		// display the render from Optix 
		PixelBuffer& pixelBuffer = camera->getPixelBuffer();
		if (pixelBuffer.uint8Pixels.size())
		{
			window->renderImage(std::move(pixelBuffer));
		}
	}

	void onCrash() override
	{
		// if we're alive maybe the user can save their work
		if(nanoguiLayer)
			nanoguiLayer->postWarningMessage("Unexpected internal error!", "Please save your work!");
	}

	void onDrop(const std::vector<std::string>& fileList) override
	{
		model.onDrop(fileList);
	}

	void onInput(const InputEvent & e) override
	{
		controller.onInput(e, camera);
	}

	void checkForErrors()
	{
		ErrMsg err = model.getNextErrorMessage();
		if (err.message.size())
		{
			switch (err.severity)
			{
				case ErrorSeverity::Information:
					nanoguiLayer->postInfoMessage("Info", err.message);
					break;

				case ErrorSeverity::Warning:
					nanoguiLayer->postWarningMessage("Warning", err.message);
					break;

				case ErrorSeverity::Critical:
					nanoguiLayer->postWarningMessage("Critical", err.message);
					break;
			}
		}
	}

	void createEngine()
	{
		engine = std::make_shared<OptixScene>(properties, config.getOptixConfig());
		engine->init(camera, config.getProgramGroups());
	}

	void onScreenGrab() { captureScreen = true; }

  private:
	  RenderLayerRef optixLayer = nullptr;
	  RenderLayerRef nanoguiLayer = nullptr;
	  OptixEngineRef engine = nullptr;

	  SceneConfig config;
	  Model model;
	  View view;
	  Controller controller;
};

Jahley::App* Jahley::CreateApplication()
{
	DesktopWindowSettings settings;
	settings.name = APP_NAME;
	settings.refreshRate = 0;
	settings.resizable = false;

	return new Application(settings, true);
}
