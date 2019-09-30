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

	// =================== picking options
	// module options
	pickingConfig.options.module_compile_options.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
	pickingConfig.options.module_compile_options.optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
	pickingConfig.options.module_compile_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_LINEINFO;

	// pipeline compile options
	pickingConfig.options.pipeline_compile_options.usesMotionBlur = false;
	pickingConfig.options.pipeline_compile_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING;
	pickingConfig.options.pipeline_compile_options.numPayloadValues = NUM_PICKING_PAYLOAD_VALUES;
	pickingConfig.options.pipeline_compile_options.numAttributeValues = 3;
	pickingConfig.options.pipeline_compile_options.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;  // TODO: should be OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW;
	pickingConfig.options.pipeline_compile_options.pipelineLaunchParamsVariableName = "params";

	// pipeline line options
	pickingConfig.options.pipeline_link_options.maxTraceDepth = 5;
	pickingConfig.options.pipeline_link_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_FULL;
	pickingConfig.options.pipeline_link_options.overrideUsesMotionBlur = false;

	// acceleration structure options
	pickingConfig.options.accel_options.buildFlags = OPTIX_BUILD_FLAG_ALLOW_COMPACTION;
	pickingConfig.options.accel_options.operation = OPTIX_BUILD_OPERATION_BUILD;

	pickingConfig.pipelineType = PipelineType::Picking;
	pickingConfig.moduleNames.add("picking");

	// =================== whitted options
	// module options
	whittedConfig.options.module_compile_options.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
	whittedConfig.options.module_compile_options.optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
	whittedConfig.options.module_compile_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_LINEINFO;

	// pipeline compile options
	whittedConfig.options.pipeline_compile_options.usesMotionBlur = false;
	whittedConfig.options.pipeline_compile_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING;
	whittedConfig.options.pipeline_compile_options.numPayloadValues = NUM_WHITTED_PAYLOAD_VALUES;
	whittedConfig.options.pipeline_compile_options.numAttributeValues = 3;
	whittedConfig.options.pipeline_compile_options.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;  // TODO: should be OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW;
	whittedConfig.options.pipeline_compile_options.pipelineLaunchParamsVariableName = "params";

	// pipeline line options
	whittedConfig.options.pipeline_link_options.maxTraceDepth = 5;
	whittedConfig.options.pipeline_link_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_FULL;
	whittedConfig.options.pipeline_link_options.overrideUsesMotionBlur = false;

	// acceleration structure options
	whittedConfig.options.accel_options.buildFlags = OPTIX_BUILD_FLAG_ALLOW_COMPACTION;
	whittedConfig.options.accel_options.operation = OPTIX_BUILD_OPERATION_BUILD;

	whittedConfig.pipelineType = PipelineType::Whitted;
	whittedConfig.moduleNames.add("viewing");
}


const json & SceneConfig::getProgramGroups(PipelineType type)
{
	switch (type)
	{
		case PipelineType::Picking:
			return getPickingPrograms();

		case PipelineType::Whitted:
			return getWhittedPrograms();

		default:
			return emptyGroup;
	}
#if 0
	progGroups[radianceHitName] = {
		{"name", radianceHitName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_HITGROUP},
		{"moduleCH", "programs" },
		{"entryFunctionNameCH", "__closesthit__ch" },
		{"moduleAH", nullptr },
		{"entryFunctionNameAH", nullptr },
		{"moduleIS", nullptr },
		{"entryFunctionNameIS", nullptr } };

	progGroups[occlusionHitName] = {
		{"name", occlusionHitName },
		{"kind", OPTIX_PROGRAM_GROUP_KIND_HITGROUP},
		{"moduleCH", "programs" },
		{"entryFunctionNameCH", "__closesthit__occlusion" },
		{"moduleAH", nullptr },
		{"entryFunctionNameAH", nullptr },
		{"moduleIS", nullptr },
		{"entryFunctionNameIS", nullptr } };

	progGroups[raygenName] = {
		{"name", raygenName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_RAYGEN},
		{"module", "programs" },
		{"entryFunctionName", "__raygen__rg" } };

	progGroups[radianceMissName] = {
		{"name", radianceMissName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_MISS},
		{"module", "programs" },
		{"entryFunctionName", "__miss__ms" } };

#endif
	
	return emptyGroup;
}

const OptixConfig& SceneConfig::getOptixConfig(PipelineType type)
{
	switch (type)
	{
		case PipelineType::Picking:
			return getPickingConfigs();

		case PipelineType::Whitted:
			return getWhittedConfigs();

		default:
			return defaultConfigs;
	}
}

const json& SceneConfig::getSimplestPrograms()
{
	
	return emptyGroup;
}

const json& SceneConfig::getPickingPrograms()
{
	
	pickingGroups[pickHitName] = {
		{"name", pickHitName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_HITGROUP},
		{"moduleCH", "picking" },
		{"entryFunctionNameCH", "__closesthit__pickHit" },
		{"moduleAH", nullptr },
		{"entryFunctionNameAH", nullptr },
		{"moduleIS", nullptr },
		{"entryFunctionNameIS", nullptr } };

	pickingGroups[pickGenName] = {
		{"name", pickGenName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_RAYGEN},
		{"module", "picking" },
		{"entryFunctionName", "__raygen__pickRay" } };

	pickingGroups[pickMissName] = {
		{"name", pickMissName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_MISS},
		{"module", "picking" },
		{"entryFunctionName", "__miss__pickMiss" } };

	return pickingGroups;
}

const json& SceneConfig::getWhittedPrograms()
{

	whittedGroups[radianceHitName] = {
		{"name", radianceHitName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_HITGROUP},
		{"moduleCH", "viewing" },
		{"entryFunctionNameCH", "__closesthit__ch" },
		{"moduleAH", nullptr },
		{"entryFunctionNameAH", nullptr },
		{"moduleIS", nullptr },
		{"entryFunctionNameIS", nullptr } };

	whittedGroups[occlusionHitName] = {
		{"name", occlusionHitName },
		{"kind", OPTIX_PROGRAM_GROUP_KIND_HITGROUP},
		{"moduleCH", "viewing" },
		{"entryFunctionNameCH", "__closesthit__occlusion" },
		{"moduleAH", nullptr },
		{"entryFunctionNameAH", nullptr },
		{"moduleIS", nullptr },
		{"entryFunctionNameIS", nullptr } };

	whittedGroups[raygenName] = {
		{"name", raygenName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_RAYGEN},
		{"module", "viewing" },
		{"entryFunctionName", "__raygen__rg" } };

	whittedGroups[radianceMissName] = {
		{"name", radianceMissName},
		{"kind", OPTIX_PROGRAM_GROUP_KIND_MISS},
		{"module", "viewing" },
		{"entryFunctionName", "__miss__ms" } };

	return whittedGroups;;
}

const OptixConfig& SceneConfig::getSimplestConfigs()
{
	return simplestdConfig;
}

const OptixConfig& SceneConfig::getPickingConfigs()
{
	return pickingConfig;
}

const OptixConfig& SceneConfig::getWhittedConfigs()
{
	return whittedConfig;
}
