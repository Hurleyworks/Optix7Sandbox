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
		LOG(INFO) << APP_NAME << " is running!";
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

  private:
	  RenderLayerRef optixLayer = nullptr;
	  RenderLayerRef nanoguiLayer = nullptr;
	  Model model;
	  View view;
	  Controller controller;
};

Jahley::App* Jahley::CreateApplication()
{
	DesktopWindowSettings settings;
	settings.name = APP_NAME;
	settings.refreshRate = 15;
	settings.resizable = false;
	settings.bgColor = Eigen::Vector4f(0.2f, 0.23f, 0.26f, 1.0f);

	return new Application(settings, true);
}
