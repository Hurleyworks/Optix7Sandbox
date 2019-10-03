// This header file was auto-generated by ClassMate++
// Created: 12 Sep 2019 6:29:23 am
// Copyright (c) 2019, HurleyWorks

#pragma once

class OptixAccel
{

 public:
	OptixAccel ();
	~OptixAccel ();
	
	void rebuildSceneAccel(ContextHandle& context, const SceneMeshes& meshes);
	OptixTraversableHandle getSceneAccel() const { return sceneAccel; }

 private:
	CUdeviceptr deviceIASoutputBuffer = 0;
	OptixTraversableHandle sceneAccel = 0;

}; // end class OptixAccel
