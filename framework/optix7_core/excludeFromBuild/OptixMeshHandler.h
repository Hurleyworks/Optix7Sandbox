// This header file was auto-generated by ClassMate++
// Created: 4 Oct 2019 5:48:01 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

class OptixMeshHandler
{

 public:
	OptixMeshHandler ();
	~OptixMeshHandler ();

	const SceneMeshes& getMeshes() const { return meshes; }
	SceneMeshes& getMeshes() { return meshes; }

	void resetMotion()
	{
		for (auto mesh : meshes)
		{
			RenderableWeakRef weakNode(mesh->getNode());
			if (weakNode.expired()) continue;

			weakNode.lock()->getSpaceTime().resetMotion();
		}
	}
	void reset()
	{
		hitgroup_records.clear();
		hitgroup_record_base.free();
		meshes.clear();
		nextSlot = 0;
		preallocate();
	}

	void onSelectionChange(OptixMeshHandle& mesh);


	CUdeviceptr getHitGroupRecordBase() { return hitgroup_record_base.get(); }
	size_t getRecordCount() const { return hitgroup_records.size(); }

	void addMesh(OptixMeshHandle mesh, OptixShaderBindingTable* const sbt, const OptixPrograms& programs);

	OptixMeshHandle findMesh(BodyID clientID)
	{
		if (clientID >= meshes.size()) return nullptr;

		// clientID is an index
		auto it = meshes.begin();
		std::advance(it, clientID);

		return *it;
	}

 private:
	SceneMeshes meshes;
	ProgramGroupHandle radianceHit = nullptr;
	ProgramGroupHandle occulsionHit = nullptr;

	CuBuffer<HitGroupRecord> hitgroup_record_base;
	const size_t hitgroup_record_size = sizeof(HitGroupRecord);
	std::vector<HitGroupRecord> hitgroup_records;
	int nextSlot = 0;

	void getPrograms(const OptixPrograms& programs);
	void preallocate();
	
}; // end class OptixMeshHandler
