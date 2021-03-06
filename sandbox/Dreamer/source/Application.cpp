#include "Jahley.h"

#include "OptixScene.h"
#include "OptixLayer.h"
#include "SceneConfig.h"
#include "Model.h"
#include "View.h"
#include "Controller.h"

const std::string APP_NAME = "Dreamer";

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
		  controller(properties),
		  view(properties)
	{
		// store the resource folder for this project
		properties.renderProps->setValue(RenderKey::ResourceFolder, getResourcePath(APP_NAME).toStdString());

		// store the resource folder shared by all projects
		properties.renderProps->setValue(RenderKey::CommonFolder, getResourcePath("Common").toStdString());

		// can't set this in App cause it knows nothing about Optix
		properties.worldProps->addDefault(WorldKey::InstanceCount, DEFAULT_PREALLOCATED_MESHES_COUNT);

		// create a camera
		camera = PerspectiveCam::create();
		float aspect = (float)settings.width / (float)settings.height;
		camera->setPerspective(DEFAULT_FOV_DEGREES, aspect, 1, 1000);
		camera->lookAt(DEFAULT_CAMERA_POSIIION, DEFAULT_CAMERA_TARGET);

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

			// create the gui overlay
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
			connect(view, &View::emitModelPath, model, &Model::loadModelFromIcon);
			connect(view, &View::emitInstances, model, &Model::createInstances);
			connect(view, &View::emitGroundPlane, model, &Model::createGroundPlane);
			connect(view, &View::emitClearScene, *this, &Application::onClearScene);
			connect(view, &View::emitFrameGrab, *this, &Application::onFrameGrab);
			connect(view, &View::emitScreenGrab, *this, &Application::onScreenGrab);
			connect(model, &Model::emitRenderable, *this, &Application::addRenderable);
			connect(model, &Model::emitRenderableList, *this, &Application::addRenderableList);
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

		if (captureScreen) 
			App::saveScreen(APP_NAME);

		// add any meshes that were loaded on the ActiveLoader thread
		Model::MeshData mesh = model.getNextLoadedMeshBuffer();
		if (mesh.first)
		{
			MeshOptions options = MeshOptions::CenterVertices | MeshOptions::NormalizeSize | MeshOptions::RestOnGround | MeshOptions::LoadStrategy;
			model.addMesh(mesh.first, mesh.second,  INVALID_ID, Pose::Identity(), Scale::Constant(1.0f), RenderableDesc(), options);
		}

		// get any images that were loaded on the ActiveLoader thread
		PixelBufferHandle image = model.getNextLoadedImage();
		if(image)
		{
			if (engine)
				engine->addImage(image);
		}

		// display the render from Optix 
		PixelBuffer& pixelBuffer = camera->getPixelBuffer();
		if (pixelBuffer.uint8Pixels.size())
		{
			if (captureRender)
			{
				model.saveRender(pixelBuffer, APP_NAME, std::chrono::system_clock::now() - startTime);
				captureRender = false;
			}
				
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
		// controller can modify Input by adding pick ray and that's ok
		controller.onInput(const_cast<InputEvent&>(e), camera);

		// don't put this in OptixLayer becasue it will
		// get called before the Controller has a chance to
		// compute a pick ray 
		if(engine)
			engine->onInput(e);
	}

	void addRenderable(RenderableNode& node)
	{
		try
		{
			if(engine)
				engine->addRenderable(node);
		}
		catch (std::exception& e)
		{
			if (nanoguiLayer)
				nanoguiLayer->postWarningMessage("Fatal Error", e.what());

			LOG(CRITICAL) << e.what();
		}
	}
	void addRenderableList(const RenderableList& nodes)
	{
		try
		{
			if (engine)
				engine->addRenderableList(std::move(nodes));
		}
		catch (std::exception & e)
		{
			if (nanoguiLayer)
				nanoguiLayer->postWarningMessage("Fatal Error", e.what());

			LOG(CRITICAL) << e.what();
		}
	}
	void checkForErrors()
	{
		if (!nanoguiLayer) return;

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
		engine = std::make_shared<OptixScene>(properties);
		engine->init(camera);

		OptixConfig dreamerConfig = config.getOptixConfig(PipelineType::Dreamer);
		engine->addPipeline(PipelineType::Dreamer, config.getProgramGroups(PipelineType::Dreamer), dreamerConfig);
	}

	void onScreenGrab() {captureScreen = true;}
	void onFrameGrab() { captureRender = true; }
	void onClearScene(bool dummy) { if(engine) engine->clearScene(); model.clearScene(); }

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
