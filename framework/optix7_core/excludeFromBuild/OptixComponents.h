﻿// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

// Program type Function name preﬁx Ray generation __raygen__ 
// Intersection __intersection__ 
// Any - hit __anyhit__
// Closest - hit __closesthit__ 
// Miss __miss__ 
// Direct callable __direct_callable__
// Continuation callable __continuation_callable__
//Exception __exception__

using juce::String;
using juce::StringArray;

const String RAYGEN_FUNCTION_PREFIX = "__raygen__";
const String INTERSECTION_FUNCTION_PREFIX = "__intersection__";
const String ANY_HIT_FUNCTION_PREFIX = "__anyhit__";
const String CLOSEST_HIT_FUNCTION_PREFIX = "__closesthit__";
const String MISS_FUNCTION_PREFIX = "__miss__";
const String DIRECT_CALLABLE__FUNCTION_PREFIX = "__direct_callable__";
const String CONTINUATION_FUNCTION_PREFIX = "__continuation_callable__";
const String EXCEPTION_FUNCTION_PREFIX = "__exception__";

struct PtxData
{
	StringArray functionNames;
	String ptxStr = String::empty;
};



struct CUDABuffer
{
	inline CUdeviceptr d_pointer() const
	{
		return (CUdeviceptr)devicePtr;
	}

	//! re-size buffer to given number of bytes
	void resize(size_t size)
	{
		if (devicePtr) free();
		alloc(size);
	}

	//! allocate to given number of bytes
	void alloc(size_t size)
	{
		assert(devicePtr == nullptr);
		this->sizeInBytes = size;
		CUDA_CHECK(cudaMalloc((void**)& devicePtr, sizeInBytes));
	}

	//! free allocated memory
	void free()
	{
		CUDA_CHECK(cudaFree(devicePtr));
		devicePtr = nullptr;
		sizeInBytes = 0;
	}

	template<typename T>
	void alloc_and_upload(const std::vector<T>& vt)
	{
		alloc(vt.size() * sizeof(T));
		upload((const T*)vt.data(), vt.size());
	}

	template<typename T>
	void upload(const T* t, size_t count)
	{
		assert(devicePtr != nullptr);
		assert(sizeInBytes == count * sizeof(T));
		CUDA_CHECK(cudaMemcpy(devicePtr, (void*)t,
			count * sizeof(T), cudaMemcpyHostToDevice));
	}

	template<typename T>
	void download(T* t, size_t count)
	{
		assert(devicePtr != nullptr);
		assert(sizeInBytes == count * sizeof(T));
		CUDA_CHECK(cudaMemcpy((void*)t, devicePtr,
			count * sizeof(T), cudaMemcpyDeviceToHost));
	}

	size_t sizeInBytes{ 0 };
	void* devicePtr{ nullptr };
};

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


using ModuleHandle = std::shared_ptr<class OptiXModule>;
class OptiXModule
{

public:
	static ModuleHandle create(const OptixPipelineCompileOptions& pipelineOptions, const OptixModuleCompileOptions& moduleOptions)
	{
		return std::make_shared<OptiXModule>(pipelineOptions, moduleOptions);
	}

public:
	OptiXModule(const OptixPipelineCompileOptions& pipelineOptions, const OptixModuleCompileOptions & moduleOptions)
		: pipelineOptions(pipelineOptions),
		  moduleOptions(moduleOptions)
	{
	}

	~OptiXModule()
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

	void creatFromPTx(ContextHandle context, const std::string & ptx)
	{
		OPTIX_CHECK_LOG(optixModuleCreateFromPTX(
			context->get(),
			&moduleOptions,
			&pipelineOptions,
			ptx.c_str(),
			ptx.size(),
			log,
			&sizeof_log,
			&module
		));

		if (sizeof_log > 1) PRINT(log);
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

	char log[2048]; // For error reporting from OptiX creation functions
	size_t sizeof_log = sizeof(log);

private:
	OptixPipelineCompileOptions pipelineOptions;
	OptixModuleCompileOptions moduleOptions;
	OptixModule module = nullptr;
};


using ProgramGroupHandle = std::shared_ptr<class OptiXProgramGroup>;

class OptiXProgramGroup
{

 public:
	static ProgramGroupHandle create(const OptixProgramGroupOptions& options, const OptixProgramGroupDesc& desc)
	{
		return std::make_shared<OptiXProgramGroup>(options, desc);
	}
	OptiXProgramGroup(const OptixProgramGroupOptions& option, const OptixProgramGroupDesc& desc)
		:options(options),
		 desc(desc)
	{
	}

	~OptiXProgramGroup()
	{
		LOG(DBUG) << _FN_;

		try
		{
			if (programGroup)
				OPTIX_CHECK(optixProgramGroupDestroy(programGroup));
		}
		catch (std::exception& e)
		{
			LOG(CRITICAL) << "Caught exception: " << e.what();
		}
	}

	void makeGroup(ContextHandle context, unsigned int groupCount = 1 )
	{
		OPTIX_CHECK_LOG(optixProgramGroupCreate(
			context->get(),
			&desc,
			groupCount,   // num program groups
			&options,
			log,
			&sizeof_log,
			&programGroup
		));

		if (sizeof_log > 1) PRINT(log);
	}

	OptixProgramGroup get()
	{
		if (programGroup)
			return programGroup;
		else
			throw std::runtime_error("OptixProgramGroup is invalid(nullptr)");
	}

	OptixProgramGroup operator -> ()
	{
		if (programGroup)
			return programGroup;
		else
			throw std::runtime_error("OptixProgramGroup is invalid(nullptr)");
	}


	char log[2048]; // For error reporting from OptiX creation functions
	size_t sizeof_log = sizeof(log);

 private:
	OptixProgramGroupOptions options = {};
	OptixProgramGroupDesc desc = {};
	OptixProgramGroup programGroup = nullptr;
};


using PipelineHandle = std::shared_ptr<class ProgramPipeline>;

class ProgramPipeline
{
	// A pipeline contains all of the programs required for a particular ray tracing launch
	// An application may use a different pipeline for each launch, or may combine multiple
	// ray-generation programs into a single pipeline.

	// Programs are ﬁrst compiled into modules of type OptixModule. One or more modules are then
	// used to create an OptixProgramGroup. Those program groups are then linked into an 
	// OptixPipeline to enable them to work together on the GPU. This is similar to the compile and 
	// link process commonly found in software development. The program groups are also used to 
	// initialize the header of the SBT record associated with those programs.
 
 public:
	static PipelineHandle create(const OptixPipelineCompileOptions& compileOptions, 
								 const OptixPipelineLinkOptions& linkOptions,
								 const std::vector<OptixProgramGroup>& programGroups)
	{
		return std::make_shared<ProgramPipeline>(compileOptions, linkOptions, programGroups);
	}

 public:
	ProgramPipeline(const OptixPipelineCompileOptions& compileOptions,
				    const OptixPipelineLinkOptions& linkOptions, 
				    const std::vector<OptixProgramGroup>& programGroups)
		: compileOptions(compileOptions),
		  linkOptions(linkOptions),
		  programGroups(programGroups)
	{
	}

	~ProgramPipeline()
	{
		LOG(DBUG) << _FN_;

		try
		{
			if (pipeline)
				OPTIX_CHECK(optixPipelineDestroy(pipeline));
		}
		catch (std::exception& e)
		{
			LOG(CRITICAL) << "Caught exception: " << e.what();
		}
	}
	OptixPipeline get()
	{
		if (pipeline)
			return pipeline;
		else
			throw std::runtime_error("ProgramPipeline is invalid(nullptr)");
	}

	void link(ContextHandle context)
	{
		 OPTIX_CHECK_LOG(optixPipelineCreate(
						context->get(),
						&compileOptions,
						&linkOptions,
					    programGroups.data(),
						 (int)programGroups.size(),
						log,
						&sizeof_log,
						&pipeline
						));

		if (sizeof_log > 1) PRINT(log);
	}

	OptixPipeline operator -> ()
	{
		if (pipeline)
			return pipeline;
		else
			throw std::runtime_error("ProgramPipeline is invalid(nullptr)");
	}

	char log[2048]; // For error reporting from OptiX creation functions
	size_t sizeof_log = sizeof(log);

 private:
	OptixPipeline pipeline = nullptr;
	OptixPipelineCompileOptions compileOptions;
	OptixPipelineLinkOptions linkOptions;
	std::vector<OptixProgramGroup> programGroups;
};


