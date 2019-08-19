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
	}

	void onInit() override
	{
		optixLayer = std::make_shared<OptixLayer>(properties);
		pushLayer(optixLayer, true);

		NanoguiLayer* const gui = new NanoguiLayer(window->glfw(), properties);
		view.create(gui);

		nanoguiLayer = RenderLayerRef(gui);
		pushOverlay(nanoguiLayer, true);
	}

	void update() override
	{
		
	}

  private:
	  RenderLayerRef optixLayer = nullptr;
	  RenderLayerRef nanoguiLayer = nullptr;
	  View view;
};

Jahley::App* Jahley::CreateApplication()
{
	DesktopWindowSettings settings;
	settings.name = APP_NAME;
	settings.refreshRate = 15;
	settings.resizable = false;
	//settings.bgColor = Eigen::Vector4f(0.2f, 0.23f, 0.26f, 1.0f);

	return new Application(settings, true);
}
