// This header file was auto-generated by ClassMate++
// Created: 20 Feb 2019 5:01:44 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

using Eigen::AlignedBox3f;

struct MeshOps
{
	static MeshBuffersHandle buildTriMeshFromArrays(const float vertices[],
													int pointCount,
													const unsigned indices[][3],
													int triCount);

	static MeshBuffersHandle createPrimitiveMesh(PrimitiveType primitive);

	static void centerVertices(MeshBuffersHandle mesh, const AlignedBox3f& modelBound, float scale);
	static void normalizeSize(MeshBuffersHandle mesh, const AlignedBox3f& modelBound, float& scale);

}; // end class MeshOps
