#include "Jahley.h"
#include "View.h"

const std::string APP_NAME = "HelloNanogui";

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings, bool windowApp = false)
		: Jahley::App(settings, windowApp),
		  view(properties, getResourcePath(APP_NAME).toStdString())
	{
		// store the resource folder for this project
		properties.renderProps->setValue(RenderKey::ResourceFolder, getResourcePath(APP_NAME).toStdString());

		// store the resource folder shared by all projects
		properties.renderProps->setValue(RenderKey::ResourceFolder, getResourcePath("Common").toStdString());
	}

	void onInit() override
	{
		NanoguiLayer* const gui = new NanoguiLayer(window->glfw(), properties);
		view.create(gui);

		nanoguiLayer = RenderLayerRef(gui);
		pushOverlay(nanoguiLayer, true);
	}

	void update() override
	{
		
	}

  private:
	  RenderLayerRef nanoguiLayer = nullptr;
	  View view;
};

Jahley::App* Jahley::CreateApplication()
{
	DesktopWindowSettings settings;
	settings.name = APP_NAME;
	settings.refreshRate = 15;
	settings.resizable = false;

	return new Application(settings, true);
}
