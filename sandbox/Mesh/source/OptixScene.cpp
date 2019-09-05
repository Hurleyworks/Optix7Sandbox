
// This source file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

#include "OptixScene.h"
#include "Renderer.h"

using Eigen::Vector4f;
using Eigen::Vector3f;

// ctor
OptixScene::OptixScene (const PropertyService& properties, const OptixConfig& config)
	: OptixEngine(properties, config)
{	

}

// dtor
OptixScene::~OptixScene ()
{	
	
}

void OptixScene::init(CameraHandle& camera, const json& programGroups)
{
	ScopedStopWatch sw(_FN_);

	renderer = Renderer::create(camera->getScreenWidth(), camera->getScreenHeight());

	pipeHandle = createPipeline(programGroups);
	if (!pipeHandle)
		throw std::runtime_error("Pipeline creation failed");

	buildSBT(camera);
}

// from Ingo Wald tutorials
// https://gitlab.com/ingowald/optix7course
void OptixScene::addRenderable(RenderableNode& node)
{
	ScopedStopWatch sw(_FN_);

	// FIXME make CUDABuffer eigen friendly instead
	// of having to copy into std::vectors
	MatrixXf V = node->getMesh()->V;
	std::vector<float> vertices(V.size());
	std::memcpy(vertices.data(), V.data(), V.size() * sizeof(float));

	vertexBuffer.alloc_and_upload(vertices);

	MatrixXu indices;
	node->getMesh()->getAllSurfaceIndices(indices);
	
	std::vector<unsigned int> triangles(indices.size());
	std::memcpy(triangles.data(), indices.data(), indices.size() * sizeof(unsigned int));

	indexBuffer.alloc_and_upload(triangles);

	// ==================================================================
   // triangle inputs
   // ==================================================================
	OptixBuildInput triangleInput = {};
	triangleInput.type = OPTIX_BUILD_INPUT_TYPE_TRIANGLES;

	// create local variables, because we need a *pointer* to the
	// device pointers
	CUdeviceptr d_vertices = vertexBuffer.d_pointer();
	CUdeviceptr d_indices = indexBuffer.d_pointer();

	triangleInput.triangleArray.vertexFormat = OPTIX_VERTEX_FORMAT_FLOAT3;
	triangleInput.triangleArray.vertexStrideInBytes = sizeof(Vector3f);
	triangleInput.triangleArray.numVertices = (int)V.cols();
	triangleInput.triangleArray.vertexBuffers = &d_vertices;

	triangleInput.triangleArray.indexFormat = OPTIX_INDICES_FORMAT_UNSIGNED_INT3;
	triangleInput.triangleArray.indexStrideInBytes = sizeof(Vector3u);
	triangleInput.triangleArray.numIndexTriplets = (int)indices.cols();
	triangleInput.triangleArray.indexBuffer = d_indices;

	uint32_t triangleInputFlags[1] = { 0 };

	// in this example we have one SBT entry, and no per-primitive
	// materials:
	triangleInput.triangleArray.flags = triangleInputFlags;
	triangleInput.triangleArray.numSbtRecords = 1;
	triangleInput.triangleArray.sbtIndexOffsetBuffer = 0;
	triangleInput.triangleArray.sbtIndexOffsetSizeInBytes = 0;
	triangleInput.triangleArray.sbtIndexOffsetStrideInBytes = 0;

	// ==================================================================
   // BLAS setup
   // ==================================================================

	OptixAccelBuildOptions accelOptions = {};
	accelOptions.buildFlags = OPTIX_BUILD_FLAG_NONE
		| OPTIX_BUILD_FLAG_ALLOW_COMPACTION
		;
	accelOptions.motionOptions.numKeys = 1;
	accelOptions.operation = OPTIX_BUILD_OPERATION_BUILD;

	OptixAccelBufferSizes blasBufferSizes;
	OPTIX_CHECK(optixAccelComputeMemoryUsage
	(context->get(),
		&config.options.accel_options,
		&triangleInput,
		1,  // num_build_inputs
		&blasBufferSizes
	));

	// ==================================================================
	// prepare compaction
	// ==================================================================

	CUDABuffer compactedSizeBuffer;
	compactedSizeBuffer.alloc(sizeof(uint64_t));

	OptixAccelEmitDesc emitDesc;
	emitDesc.type = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE;
	emitDesc.result = compactedSizeBuffer.d_pointer();

	// ==================================================================
	// execute build (main stage)
	// ==================================================================

	CUDABuffer tempBuffer;
	tempBuffer.alloc(blasBufferSizes.tempSizeInBytes);

	CUDABuffer outputBuffer;
	outputBuffer.alloc(blasBufferSizes.outputSizeInBytes);

	OPTIX_CHECK(optixAccelBuild(context->get(),
		/* stream */0,
		&config.options.accel_options,
		&triangleInput,
		1,
		tempBuffer.d_pointer(),
		tempBuffer.sizeInBytes,

		outputBuffer.d_pointer(),
		outputBuffer.sizeInBytes,

		&gAccel,

		&emitDesc, 1
	));
	CUDA_SYNC_CHECK();

	// perform compaction
	// ==================================================================
	uint64_t compactedSize;
	compactedSizeBuffer.download(&compactedSize, 1);

	asBuffer.alloc(compactedSize);
	OPTIX_CHECK(optixAccelCompact(context->get(),
		/*stream:*/0,
		gAccel,
		asBuffer.d_pointer(),
		asBuffer.sizeInBytes,
		&gAccel));
	CUDA_SYNC_CHECK();

	// ==================================================================
	// aaaaaand .... clean up
	// ==================================================================
	outputBuffer.free(); // << the UNcompacted, temporary output buffer
	tempBuffer.free();
	compactedSizeBuffer.free();
}

void OptixScene::buildSBT(CameraHandle& camera)
{
	CUdeviceptr  raygen_record;
	const size_t raygen_record_size = sizeof(RayGenSbtRecord);
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&raygen_record), raygen_record_size));

	RayGenSbtRecord rg_sbt;
	updateCamera(camera, rg_sbt);
	
	OPTIX_CHECK(optixSbtRecordPackHeader(config.programs.raygenProgs.front()->get(), &rg_sbt));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(raygen_record),
		&rg_sbt,
		raygen_record_size,
		cudaMemcpyHostToDevice
	));

	CUdeviceptr miss_record;
	size_t      miss_record_size = sizeof(MissSbtRecord);
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&miss_record), miss_record_size));

	// get the miss color from the user
	MissSbtRecord ms_sbt;
	Vector4f bg = properties.renderProps->getVal<Vector4f>(RenderKey::BackgroundColor);
	ms_sbt.data.r = bg.x();
	ms_sbt.data.b = bg.y();
	ms_sbt.data.g = bg.z();

	OPTIX_CHECK(optixSbtRecordPackHeader(config.programs.missProgs.front()->get(), &ms_sbt));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(miss_record),
		&ms_sbt,
		miss_record_size,
		cudaMemcpyHostToDevice
	));

	CUdeviceptr hitgroup_record;
	size_t      hitgroup_record_size = sizeof(HitGroupSbtRecord);
	CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&hitgroup_record), hitgroup_record_size));

	// get the mesh color from the user
	HitGroupSbtRecord hg_sbt;
	Vector4f col = properties.renderProps->getVal<Vector4f>(RenderKey::MeshColor);
	hg_sbt.data.r = col.x();
	hg_sbt.data.g = col.y();
	hg_sbt.data.b = col.z();

	OPTIX_CHECK(optixSbtRecordPackHeader(config.programs.hitgroupProgs.front()->get(), &hg_sbt));
	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(hitgroup_record),
		&hg_sbt,
		hitgroup_record_size,
		cudaMemcpyHostToDevice
	));

	sbt.raygenRecord = raygen_record;
	sbt.missRecordBase = miss_record;
	sbt.missRecordStrideInBytes = sizeof(MissSbtRecord);
	sbt.missRecordCount = 1;
	sbt.hitgroupRecordBase = hitgroup_record;
	sbt.hitgroupRecordStrideInBytes = sizeof(HitGroupSbtRecord);
	sbt.hitgroupRecordCount = 1;
}

void OptixScene::syncCamera(CameraHandle& camera)
{
	RayGenSbtRecord rg_sbt;
	optixSbtRecordPackHeader(config.programs.raygenProgs.front()->get(), &rg_sbt);

	updateCamera(camera, rg_sbt);

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.raygenRecord),
		&rg_sbt,
		sizeof(RayGenSbtRecord),
		cudaMemcpyHostToDevice
	));
}

void OptixScene::updateCamera(CameraHandle& camera, RayGenSbtRecord& rg_sbt)
{
	// recalc the view matrix
	if(camera->isDirty())
		camera->getViewMatrix();

	const Vector3f& eye = camera->getEyePoint();
	const Vector3f& forward = camera->getFoward();
	const Vector3f& right = camera->getRight();
	const Vector3f& up = camera->getUp();

	float ulen, vlen, wlen;
	wlen = forward.norm();
	vlen = wlen * tanf(0.5f * camera->getFOV() * M_PIf / 180.0f);
	ulen = vlen * camera->getAspect();;

	float3 camRight, camUp, camForward, camEye;
	camUp = make_float3(up.x(), up.y(), up.z());
	camUp *= vlen;

	camRight = make_float3(right.x(), right.y(), right.z());
	camRight *= ulen;

	camForward = make_float3(forward.x(), forward.y(), forward.z());
	camEye = make_float3(eye.x(), eye.y(), eye.z());

	rg_sbt.data.cam_eye = camEye;
	rg_sbt.data.camera_u = camRight;
	rg_sbt.data.camera_v = camUp;
	rg_sbt.data.camera_w = camForward;
}

void OptixScene::syncBackgoundColor()
{
	Vector4f bg = properties.renderProps->getVal<Vector4f>(RenderKey::BackgroundColor);

	MissSbtRecord ms_sbt;
	optixSbtRecordPackHeader(config.programs.missProgs.front()->get(), &ms_sbt);
	ms_sbt.data.r = bg.x();
	ms_sbt.data.g = bg.y();
	ms_sbt.data.b = bg.z();

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.missRecordBase),
		&ms_sbt,
		sizeof(MissSbtRecord),
		cudaMemcpyHostToDevice
	));
}

void OptixScene::syncMeshColor()
{
	Vector4f col = properties.renderProps->getVal<Vector4f>(RenderKey::MeshColor);

	HitGroupSbtRecord hg_sbt;
	optixSbtRecordPackHeader(config.programs.hitgroupProgs.front()->get(), &hg_sbt);

	hg_sbt.data.r = col.x();
	hg_sbt.data.g = col.y();
	hg_sbt.data.b = col.z();

	CUDA_CHECK(cudaMemcpy(
		reinterpret_cast<void*>(sbt.hitgroupRecordBase),
		&hg_sbt,
		sizeof(HitGroupSbtRecord),
		cudaMemcpyHostToDevice
	));
}
