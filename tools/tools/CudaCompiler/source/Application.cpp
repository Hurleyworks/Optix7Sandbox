#include "Jahley.h"

#include "Model.h"
#include "View.h"

const std::string APP_NAME = "CudaCompiler";

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings, bool windowApp = false)
		: Jahley::App(settings, windowApp),
		  view(properties)
	{
		// store the resource folder for this project
		properties.renderProps->setValue(RenderKey::ResourceFolder, getResourcePath(APP_NAME).toStdString());

		// store the resource folder shared by all projects
		properties.renderProps->setValue(RenderKey::CommonFolder, getResourcePath("Common").toStdString());

		// no need for performance graph
		properties.renderProps->setValue(RenderKey::ShowPerformanceGraph, false);
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

			// hook up View and Model via signal/slots
			connect(view, &View::emitCudaFolder, model, &Model::findCudaFiles);
			connect(view, &View::emitPtxFolder, model, &Model::setPtxOutputFolder);
			connect(view, &View::emitCompile, model, &Model::runNVCC);
			connect(view, &View::emitIncludeFolder, model, &Model::addIncludePath);
			connect(view, &View::emitReset, model, &Model::reset);
		}
		catch (std::exception& e)
		{
			if (nanoguiLayer)
				nanoguiLayer->postWarningMessage("Fatal Error", e.what());

			LOG(CRITICAL) << e.what();
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

  private:
	  RenderLayerRef nanoguiLayer = nullptr;
	  Model model;
	  View view;
};

Jahley::App* Jahley::CreateApplication()
{
	DesktopWindowSettings settings;
	settings.width = 600;
	settings.height = 400;
	settings.name = APP_NAME;
	settings.refreshRate = 15;
	settings.resizable = false;

	return new Application(settings, true);
}
