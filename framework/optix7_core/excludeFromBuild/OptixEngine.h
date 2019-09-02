// This header file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

using OptixEngineRef = std::shared_ptr<class OptixEngine>;

using juce::StringArray;
using juce::String;
using sabi::CameraHandle;
using sabi::RenderableNode;

class OptixEngine : public std::enable_shared_from_this<OptixEngine>, protected Noncopyable
{
	
 public:
	OptixEngineRef getPtr() { return shared_from_this(); }

	using ProgramDB = std::unordered_map<String, PtxData>;
	using Modules = std::vector<ModuleHandle>;

 public:
	virtual ~OptixEngine ();

	virtual void init(CameraHandle& camera) = 0;
	virtual void render(CameraHandle& camera) = 0;
	virtual void buildSBT(CameraHandle& camera) = 0;
	virtual void addRenderable(RenderableNode& node) {}

	const OptixPipeline getPipeline() const { return pipeHandle->get(); }
	const OptixShaderBindingTable* getSBT() const { return &sbt; }
	const OptixTraversableHandle& getGAS() const { return gAccel; }

 protected:
	OptixEngine(const PropertyService& properties, const OptixConfig& config);

	PropertyService properties;
	OptixConfig config;

	StringArray progFuncNames;  
	StringArray programPrefixes;
	ProgramDB programDB;
	Modules modules;

	ContextHandle context = nullptr;
	PipelineHandle pipeHandle = nullptr;
	OptixShaderBindingTable sbt = {};
	OptixTraversableHandle gAccel= 0; 

	PipelineHandle createPipeline();

private:
	ModuleHandle createModule(PtxData& data);
	ProgramGroupHandle createRaygenPrograms(ModuleHandle& module, const String& functionName);
	ProgramGroupHandle createMissPrograms(ModuleHandle& module, const String& functionName);
	ProgramGroupHandle createHitgroupPrograms(ModuleHandle& module, const StringArray& functionNames);
	
	void createProgramDatabase();
	void createProgramGoups();
	String extractProgramFunctionName(const String& line, const String& prefix);

}; // end class OptixEngine
