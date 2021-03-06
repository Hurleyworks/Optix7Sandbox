// This header file was auto-generated by ClassMate++
// Created: 2 Sep 2019 5:16:51 am
// Copyright (c) 2019, HurleyWorks

#pragma  once

using sabi::Surface;
using sabi::MeshBuffers;
using sabi::MeshBuffersHandle;
using juce::File;

using igl::read_triangle_mesh;
using igl::per_vertex_normals;


class ActiveLoader 
{

 public:
	ActiveLoader() = default;
	~ActiveLoader() = default;
	
	void loadMesh(const std::string& path, LoadMeshCallback& callback)
	{
		/*File f(path);
		if (f.getFileExtension() == ".gltf")
		{
			return loadGLTF(path, callback);
		}*/
		Eigen::MatrixXd V;
		Eigen::MatrixXi F;
		if (!read_triangle_mesh(path, V, F))
		{
			LOG(CRITICAL) << "load mesh failed for: " << path;
			return;
		}

		MeshBuffersHandle m = std::make_shared<MeshBuffers>();
		m->V = V.transpose().cast<float>(); // Libigl needs to be transposed

		// Compute per-vertex normals
		Eigen::MatrixXd N;
		per_vertex_normals(V, F, N);
		m->N = N.transpose().cast<float>(); // Libigl needs to be transposed

		MatrixXu indices = F.cast<unsigned>();
		Surface s;
		s.indices() = indices.transpose(); // Libigl needs to be transposed
		m->S.push_back(s);

		callback(m, path);
	}

	
#if 0
	
	void loadGLTF(const std::string& path, LoadMeshCallback& callback)
	{
		ScopedStopWatch sw(_FN_);

		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		tinygltf::Model model;
		bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);

		if (!warn.empty()) 
			LOG(WARNING) <<  warn.c_str();

		if (!err.empty()) 
			LOG(CRITICAL) << err.c_str();

		if (!ret) 
			throw std::runtime_error("Failed to parse glTF: " + path);

		for (const auto& gltf_buffer : model.buffers)
		{
			const uint64_t buf_size = gltf_buffer.data.size();
			std::cerr << "Processing glTF buffer '" << gltf_buffer.name << "'\n"
				<< "\tbyte size: " << buf_size << "\n"
				<< "\turi      : " << gltf_buffer.uri << std::endl;

			//scene.addBuffer(buf_size, gltf_buffer.data.data());
		}

		glTFLoader gltf;
		gltf.processNodes(model, callback);
		
	}

#endif

}; // end class ActiveLoader
