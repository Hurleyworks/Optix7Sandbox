// This source file was auto-generated by ClassMate++
// Created: 29 Aug 2019 3:54:59 pm
// Copyright (c) 2019, HurleyWorks

#include "SceneConfig.h"

using juce::File;

// ctor
SceneConfig::SceneConfig (const PropertyService& properties)
	: properties(properties)
{	
}

// dtor
SceneConfig::~SceneConfig ()
{	
	
}

void SceneConfig::init()
{
	configFolder = String(properties.renderProps->getVal<std::string>(RenderKey::ResourceFolder));
	File f(configFolder);
	if (!f.exists())
		throw std::runtime_error("No resource folder found at " + configFolder.toStdString());

	// =================== dreamer options
	// module options
	dreamerConfig.options.module_compile_options.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
	dreamerConfig.options.module_compile_options.optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
	dreamerConfig.options.module_compile_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_LINEINFO;

	// pipeline compile options
	dreamerConfig.options.pipeline_compile_options.usesMotionBlur = false;
	dreamerConfig.options.pipeline_compile_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING;
	dreamerConfig.options.pipeline_compile_options.numPayloadValues = NUM_DREAMER_PAYLOAD_VALUES;
	dreamerConfig.options.pipeline_compile_options.numAttributeValues = 3;
	dreamerConfig.options.pipeline_compile_options.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;  // TODO: should be OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW;
	dreamerConfig.options.pipeline_compile_options.pipelineLaunchParamsVariableName = "sysData";

	// pipeline line options
	dreamerConfig.options.pipeline_link_options.maxTraceDepth = 5;
	dreamerConfig.options.pipeline_link_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_FULL;
	dreamerConfig.options.pipeline_link_options.overrideUsesMotionBlur = false;

	// acceleration structure options
	dreamerConfig.options.accel_options.buildFlags = OPTIX_BUILD_FLAG_ALLOW_COMPACTION;
	dreamerConfig.options.accel_options.operation = OPTIX_BUILD_OPERATION_BUILD;

	dreamerConfig.pipelineType = PipelineType::Whitted;
	dreamerConfig.moduleNames.add("dreamer");
}


const json & SceneConfig::getProgramGroups(PipelineType type)
{
	switch (type)
	{
		case PipelineType::Dreamer:
			return getDreamerPrograms();

		default:
			return emptyGroup;
	}

	return emptyGroup;
}

const OptixConfig& SceneConfig::getOptixConfig(PipelineType type)
{
	switch (type)
	{
		case PipelineType::Dreamer:
			return getDreamerConfigs();

		default:
			return defaultConfigs;
	}
}

const json& SceneConfig::getDreamerPrograms()
{

	dreamerGroups[occlusionHitName] = {
		{"name", occlusionHitName },
		{"kind", OPTIX_PROGRAM_GROUP_KIND_HITGROUP},
		{"moduleCH", "dreamer" },
		{"entryFunctionNameCH", "__closesthit__occlusion" },
		{"moduleAH", nullptr },
		{"entryFunctionNameAH", nullptr },
		{"moduleIS", nullptr },
		{"entryFunctionNameIS", nullptr } };

	dreamerGroups[radianceHitName] = {
		{"name", radianceHitName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_HITGROUP},
		{"moduleCH", "dreamer" },
		{"entryFunctionNameCH", "__closesthit__radiance" },
		{"moduleAH", nullptr },
		{"entryFunctionNameAH", nullptr },
		{"moduleIS", nullptr },
		{"entryFunctionNameIS", nullptr } };

	dreamerGroups[raygenName] = {
		{"name", raygenName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_RAYGEN},
		{"module", "dreamer" },
		{"entryFunctionName", "__raygen__eye_path" } };

	dreamerGroups[radianceMissName] = {
		{"name", radianceMissName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_MISS},
		{"module", "dreamer" },
		{"entryFunctionName", "__miss__miss" } };

	dreamerGroups[exceptionName] = {
		{"name", exceptionName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_EXCEPTION},
		{"module", "dreamer" },
		{"entryFunctionName", "__exception__all" } };

	return dreamerGroups;;
}
