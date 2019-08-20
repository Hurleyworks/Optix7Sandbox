#include "Jahley.h"

#include "Model.h"
#include "View.h"
#include "Controller.h"

const std::string APP_NAME = "HelloOptix";


class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings, bool windowApp = false)
		: Jahley::App(settings, windowApp),
		  view(properties)
	{
		// aquire the Resources folder
		properties.renderProps->setValue(RenderKey::ResourceFolder, getResourcePath(APP_NAME).toStdString());
	}

	void onInit() override
	{
		try
		{
			// create the Gui overlay
			NanoguiLayer* const gui = new NanoguiLayer(window->glfw(), properties);
			view.create(gui);
			nanoguiLayer = RenderLayerRef(gui);
			pushOverlay(nanoguiLayer, true);

			// create the Optix renderer
			optixLayer = std::make_shared<OptixLayer>(properties);
			pushLayer(optixLayer, true);
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
		if (!splashScreenLoaded)
		{
			// the one and only JahleyBlue
			const String fileName = getResourcePath("Common") + "/image/splash.jpg";
			loadImage(fileName.toStdString());

			splashScreenLoaded = true;
		}
	}

	void onCrash() override
	{
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

	void loadImage(const std::string& path)
	{
		ImagePixels splash;
		ImageInfo spec;
		model.loadImage(path, splash, spec);
		
		if (splash.size())
		{
			window->renderImage(std::move(splash), spec);
		}
		else
		{
			LOG(CRITICAL) << "Failed to load splash from " << path;
		}
	}

  private:
	  RenderLayerRef optixLayer = nullptr;
	  RenderLayerRef nanoguiLayer = nullptr;
	  Model model;
	  View view;
	  Controller controller;

	  // splash screen
	  bool splashScreenLoaded = false;
};

Jahley::App* Jahley::CreateApplication()
{
	DesktopWindowSettings settings;
	settings.name = APP_NAME;
	settings.refreshRate = 15;
	settings.resizable = false;

	return new Application(settings, true);
}
