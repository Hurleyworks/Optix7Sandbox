#include "Jahley.h"

#include "OptixScene.h"
#include "OptixLayer.h"

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
			// create the Gui overlay
			NanoguiLayer* const gui = new NanoguiLayer(window->glfw(), properties, camera);
			view.create(gui);
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
		// module options
		config.options.module_compile_options.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
		config.options.module_compile_options.optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
		config.options.module_compile_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_LINEINFO;

		// pipeline compile options
		config.options.pipeline_compile_options.usesMotionBlur = false;
		config.options.pipeline_compile_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS;
		config.options.pipeline_compile_options.numPayloadValues = 3;
		config.options.pipeline_compile_options.numAttributeValues = 3;
		config.options.pipeline_compile_options.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;  // TODO: should be OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW;
		config.options.pipeline_compile_options.pipelineLaunchParamsVariableName = "params";

		// pipeline line options
		config.options.pipeline_link_options.maxTraceDepth = 5;
		config.options.pipeline_link_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_FULL;
		config.options.pipeline_link_options.overrideUsesMotionBlur = false;

		// acceleration structure options
		config.options.accel_options.buildFlags = OPTIX_BUILD_FLAG_ALLOW_COMPACTION;
		config.options.accel_options.operation = OPTIX_BUILD_OPERATION_BUILD;

		engine = std::make_shared<OptixScene>(properties, config);
		engine->init(camera);
	}

  private:
	  RenderLayerRef optixLayer = nullptr;
	  RenderLayerRef nanoguiLayer = nullptr;
	  CameraHandle camera = nullptr;
	  OptixConfig config;
	  OptixEngineRef engine = nullptr;

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
