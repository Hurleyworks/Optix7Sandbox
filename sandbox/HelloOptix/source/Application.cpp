#include "Jahley.h"

const std::string APP_NAME = "HelloOptix";

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings, bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
	}

	void onInit() override
	{
		optixLayer = std::make_shared<OptixLayer>();
		pushLayer(optixLayer, true);

		nanoguiLayer = std::make_shared<NanoguiLayer>(window->glfw());
		pushOverlay(nanoguiLayer, true);
	}

	void update() override
	{
		
	}

  private:
	  RenderLayerRef optixLayer = nullptr;
	  RenderLayerRef nanoguiLayer = nullptr;
};

Jahley::App* Jahley::CreateApplication()
{
	DesktopWindowSettings settings;
	settings.name = APP_NAME;
	settings.refreshRate = 15;
	settings.bgColor = Eigen::Vector4f(0.2f, 0.23f, 0.26f, 1.0f);

	return new Application(settings, true);
}
