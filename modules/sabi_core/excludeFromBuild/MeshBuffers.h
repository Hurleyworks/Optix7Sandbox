// This header file was auto-generated by ClassMate++
// Created: 10 Feb 2018 9:21:04 am
// Copyright (c) 2018, HurleyWorks

#pragma  once

struct MeshBuffers
{
	Surfaces S;   // collection of triangles with different materials
	MatrixXf V;   // vertices
	MatrixXf N;   // vertex normals
	VMapDB vmaps; // collection of vertex maps like UV or Weight

	size_t faceCount = 0; // must be computed
	size_t pointCount() const { return V.cols(); }
	size_t triangleCount() const { return faceCount; }
	
	void reset()
	{
		V.resize(3, 0);
		N.resize(3, 0);
		S.clear();
		vmaps.clear();
	}

	void getAllSurfaceIndices(MatrixXu & allIndices)
	{
		faceCount = 0;
		for (auto s : S)
		{
			faceCount += s.triangleCount();
		}

		allIndices.resize(3, faceCount);

		int index = 0;
		for (auto s : S)
		{
			size_t triCount = s.triangleCount();

			for (int i = 0; i < triCount; i++)
				allIndices.col(index++) = s.indices().col(i);
		}
	}

	void packIntoBinarStream(std::stringstream & str)
	{
		BinaryWriter writer(str);

		// pack all the surface indices into the binary stream
		for (auto s : S)
		{
			const MatrixXu& F = s.indices();
			for (int i = 0; i < F.cols(); i++)
			{
				for (int j = 0; j < F.rows(); j++)
				{
					// must make this assignment first 
					// or it fails for some reason
					// writer << F.col(i)[j]; FAILS
					int index = F.col(i)[j];
					writer << index;
				}
			}
		}

		// pack all the vertices into the binary stream
		for (int i = 0; i < V.cols(); i++)
		{
			for (int j = 0; j < V.rows(); j++)
			{
				// must make this assignment first 
				// or it fails for some reason
				// writer << V.col(i)[j]; FAILS
				float f = V.col(i)[j];
				writer << f;
			}
		}

		// pack all the vertex normals into the binary stream
		for (int i = 0; i < N.cols(); i++)
		{
			for (int j = 0; j < N.rows(); j++)
			{
				// must make this assignment first 
				// or it fails for some reason
				float f = N.col(i)[j];
				writer << f;
			}
		}

		// pack all the vertex maps into the binary stream
		for (auto it : vmaps)
		{
			const MatrixXf& V = it.second->values;
			for (int i = 0; i < V.cols(); i++)
			{
				for (int j = 0; j < V.rows(); j++)
				{
					// must make this assignment first 
					// or it fails for some reason
					float f = V.col(i)[j];
					writer << f;
				}
			}
		}
	}
};

using MeshBuffersHandle = std::shared_ptr<MeshBuffers>;