// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

#pragma once


struct Params
{
	uchar4* image;
	uint32_t               image_width;
	uint32_t               image_height;
	int32_t                origin_x;
	int32_t                origin_y;
	OptixTraversableHandle handle;
};


struct RayGenData
{
	float3 cam_eye;
	float3 camera_u, camera_v, camera_w;
};


struct MissData
{
	float r, g, b;
};


struct HitGroupData
{
	float radius;
};

template <typename T>
struct SbtRecord
{
	__align__(OPTIX_SBT_RECORD_ALIGNMENT) char header[OPTIX_SBT_RECORD_HEADER_SIZE];
	T data;
};

typedef SbtRecord<RayGenData>     RayGenSbtRecord;
typedef SbtRecord<MissData>       MissSbtRecord;
typedef SbtRecord<HitGroupData>   HitGroupSbtRecord;

using ContextHandle = std::shared_ptr<class OptixContext>;

class OptixContext
{

 public:
	static ContextHandle create(const OptixDeviceContextOptions & options) { return std::make_shared<OptixContext>(options); }

 public:
	OptixContext(const OptixDeviceContextOptions& options)
	{
		try
		{
			// Initialize CUDA
			CUDA_CHECK(cudaFree(0));

			CUcontext cuCtx = 0;  // zero means take the current context
			OPTIX_CHECK(optixInit());
			OPTIX_CHECK(optixDeviceContextCreate(cuCtx, &options, &context));
		}
		catch (std::exception& e)
		{
			LOG(CRITICAL) << "Caught exception: " << e.what();

			if (context)
				OPTIX_CHECK(optixDeviceContextDestroy(context));

			context = nullptr;
	
		}
	}
	~OptixContext()
	{
		LOG(DBUG) << _FN_;

		try
		{
			if(context)
				OPTIX_CHECK(optixDeviceContextDestroy(context));
		}
		catch (std::exception& e)
		{
			LOG(CRITICAL) << "Caught exception: " << e.what();
		}
	}

	OptixDeviceContext get()
	{
		if (context)
			return context;
		else
			throw std::runtime_error("OptixDeviceContext is invalid(nullptr)");
	}

	OptixDeviceContext operator -> ()
	{
		if (context)
			return context;
		else
			throw std::runtime_error("OptixDeviceContext is invalid(nullptr)");
	}


 private:
	OptixDeviceContext context = nullptr;
};

using ModuleHandle = std::shared_ptr<class OptixMod>;

class OptixMod
{
	
 public:
	static ModuleHandle create() { return std::make_shared<OptixMod>(); }

public:
	OptixMod()
	{
		try
		{
			module_compile_options.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
			module_compile_options.optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
			module_compile_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_LINEINFO;

			pipeline_compile_options.usesMotionBlur = false;
			pipeline_compile_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING;
			pipeline_compile_options.numPayloadValues = 2;
			pipeline_compile_options.numAttributeValues = 2;
			pipeline_compile_options.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;  // TODO: should be OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW;
			pipeline_compile_options.pipelineLaunchParamsVariableName = "params";
		}
		catch (std::exception& e)
		{
			LOG(CRITICAL) << e.what();
			if (module)
			{ 
				OPTIX_CHECK(optixModuleDestroy(module));
				module = nullptr; // FIXME do I really need to?
			}
		}
	}
	~OptixMod()
	{
		LOG(DBUG) << _FN_;

		try
		{
			if (module)
				OPTIX_CHECK(optixModuleDestroy(module));
		}
		catch (std::exception& e)
		{
			LOG(CRITICAL) << "Caught exception: " << e.what();
		}
	}

	void setPipelineOptions(OptixPipelineCompileOptions options)
	{
		pipeline_compile_options = options;
	}



	void createFromPtx(ContextHandle ctx, const std::string& ptx)
	{
		OPTIX_CHECK_LOG(optixModuleCreateFromPTX(
			ctx->get(),
			&module_compile_options,
			&pipeline_compile_options,
			ptx.c_str(),
			ptx.size(),
			log,
			&sizeof_log,
			&module
		));
	}

	OptixModule get()
	{
		if (module)
			return module;
		else
			throw std::runtime_error("OptixModule is invalid(nullptr)");
	}

	OptixModule operator -> ()
	{
		if (module)
			return module;
		else
			throw std::runtime_error("OptixModule is invalid(nullptr)");
	}

private:
	OptixModule module = nullptr;
	OptixPipelineCompileOptions pipeline_compile_options = {};
	OptixModuleCompileOptions module_compile_options = {};
	char log[2048];
	size_t sizeof_log = sizeof(log);
};

using Modules = std::vector<ModuleHandle>;
