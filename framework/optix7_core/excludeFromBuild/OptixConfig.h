#pragma once

struct OptixOptions
{
	OptixDeviceContextOptions context_options = {};
	OptixAccelBuildOptions accel_options = {};
	OptixPipelineCompileOptions pipeline_compile_options = {};
	OptixPipelineLinkOptions pipeline_link_options = {};
	OptixProgramGroupOptions program_group_options = {}; 
	OptixModuleCompileOptions module_compile_options = {};
	
};

struct OptixDesc
{
	OptixAccelEmitDesc emitProperty = {};
	OptixProgramGroupDesc raygen_prog_group_desc = {};
	OptixProgramGroupDesc miss_prog_group_desc = {};
	OptixProgramGroupDesc hitgroup_prog_group_desc = {};
};

struct OptixPrograms
{
	std::string ptx = "";
	OptixProgramGroup raygen_prog_group = nullptr;
	OptixProgramGroup miss_prog_group = nullptr;
	OptixProgramGroup hitgroup_prog_group = nullptr;
};

struct OptixConfig
{
	OptixOptions options;
	OptixDesc desc;
	OptixPrograms programs;
};