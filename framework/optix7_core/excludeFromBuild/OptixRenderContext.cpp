
// This header file was auto-generated by ClassMate++
// Created: 26 Sep 2019 2:36:23 pm
// Copyright (c) 2019, HurleyWorks

OptixRenderContext::OptixRenderContext(PipelineType type)
	: type(type)
{
	CUDA_CHECK(cudaStreamCreate(&stream));
}

OptixRenderContext::~OptixRenderContext()
{
	try
	{
		CUDA_CHECK(cudaStreamDestroy(stream));

		CUDA_CHECK(cudaFree(reinterpret_cast<void*>(sbt.raygenRecord)));
		CUDA_CHECK(cudaFree(reinterpret_cast<void*>(sbt.missRecordBase)));

		// Don't free sbt.hitgroupRecordBase here because it's already 
		// been freed in the OptixMeshHandler destructor
		//CUDA_CHECK(cudaFree(reinterpret_cast<void*>(sbt.hitgroupRecordBase)));

	}
	catch (const std::runtime_error& e)
	{
		LOG(CRITICAL) << e.what();
	}
	catch (...)
	{
		LOG(CRITICAL) << "Caught unknow exception";
	}

}