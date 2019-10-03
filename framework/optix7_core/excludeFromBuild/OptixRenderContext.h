// This header file was auto-generated by ClassMate++
// Created: 26 Sep 2019 2:36:23 pm
// Copyright (c) 2019, HurleyWorks

#pragma  once

using Eigen::Vector3i;
using sabi::CameraHandle;

using OptixRenderContextHandle = std::shared_ptr<class OptixRenderContext>;

class OptixRenderContext
{
	// CUstream
	// Applications manage the concurrent operations described above through streams.
	// A stream is a sequence of commands(possibly issued by different host threads) 
	// that execute in order.Different streams, on the other hand, may execute their 
	// commands out of order with respect to one another or concurrently; this behavior 
	// is not guaranteed and should therefore not be relied upon for correctness
	// (e.g., inter - kernel communication is undefined).

 public:

	virtual ~OptixRenderContext();
	
	template<typename T>
	void uploadLaunchParameters(T& params)
	{
		deviceParams.upload(&params, 1);
	}

	virtual void createRaygenRecord(const OptixEngineRef& engine) = 0;
	virtual void createMissRecord(const OptixEngineRef& engine) = 0;
	virtual void createEmptyHitGroupRecord(const OptixEngineRef& engine) = 0;
	virtual void rebuildHitgroupSBT(const SceneMeshes& meshes) = 0;

	virtual void initializeLaunchParams() = 0;
	virtual void preLaunch(CameraHandle& camera, OptixEngineRef& engine, InputEvent& input) = 0;
	virtual void updateCamera(CameraHandle& camera) = 0;
	virtual void postLaunch(CameraHandle& camera, OptixEngineRef& engine, InputEvent& input) = 0;

	virtual void launch(OptixEngineRef& engine)
	{
		OPTIX_CHECK(optixLaunch(pipeline->get(), stream, deviceParams.d_pointer(), deviceParams.sizeInBytes, &sbt, size.x(), size.y(), size.z()));
	}

	void setConfig(const OptixConfig& config) { this->config = config; }
	void setPipeline(PipelineHandle& handle) { pipeline = handle; }
	void setSceneAccel(OptixTraversableHandle IAS) { sceneAccel = IAS; }
	void setDimensions(const Vector3i& size) { this->size = size; }

	OptixShaderBindingTable* getSBT() { return &sbt; }
	const OptixConfig& getConfig() const { return config; }

 protected:
	OptixRenderContext();

	CUDABuffer deviceParams;
	OptixConfig config;
	OptixShaderBindingTable sbt = {};
	OptixTraversableHandle sceneAccel = 0;
	PipelineHandle pipeline = nullptr;
	Vector3i size = Vector3i(DEFAULT_DESKTOP_WINDOW_WIDTH, DEFAULT_DESKTOP_WINDOW_HEIGHT, 1);
	CUstream stream = 0;
};