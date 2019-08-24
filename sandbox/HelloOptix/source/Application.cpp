#include "Jahley.h"

#include "OptixLayer.h"
#include "Model.h"
#include "View.h"
#include "Controller.h"

const std::string APP_NAME = "HelloOptix";

using sabi::PixelBuffer;
using sabi::PerspectiveCam;

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings, bool windowApp = false)
		: Jahley::App(settings, windowApp),
		  model(properties),
		  view(properties)
	{
		// store the resource folder for this project
		properties.renderProps->setValue(RenderKey::ResourceFolder, getResourcePath(APP_NAME).toStdString());

		// store the resource folder shared by all projects
		properties.renderProps->setValue(RenderKey::ResourceFolder, getResourcePath("Common").toStdString());

		// create a camera
		camera = PerspectiveCam::create();
		float aspect = (float)settings.width / (float)settings.height;
		camera->setPerspective(DEFAULT_FOV_DEGREES, aspect, 1, 1000);
		camera->lookAt(DEFAULT_CAMERA_POSIIION, DEFAULT_CAMERA_TARGET);
	}

	void onInit() override
	{
		try
		{
			// create the Gui overlay
			NanoguiLayer* const gui = new NanoguiLayer(window->glfw(), properties, camera);
			view.create(gui);
			nanoguiLayer = RenderLayerRef(gui);
			pushOverlay(nanoguiLayer, true);

			// create the Optix layer
			optixLayer = std::make_shared<OptixLayer>(properties, camera);
			pushLayer(optixLayer, true);

			connect(view, &View::emitPrimitiveType, model, &Model::loadPrimitive);
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
		ErrMsg err = model.getNextErrorMessage();
		if (err.errMessage.size())
		{
			switch (err.severity)
			{
				case ErrorSeverity::Information:
					nanoguiLayer->postInfoMessage("Info", err.errMessage);
					break;
				case ErrorSeverity::Warning:
					nanoguiLayer->postInfoMessage("Warning", err.errMessage);
					break;
			}
		}

		// display the render from the Optix 
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
		controller.onInput(e);
	}

  private:
	  RenderLayerRef optixLayer = nullptr;
	  RenderLayerRef nanoguiLayer = nullptr;
	  CameraHandle camera = nullptr;

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
