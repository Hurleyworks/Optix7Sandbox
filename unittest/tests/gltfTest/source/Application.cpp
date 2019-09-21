#include "Jahley.h"

#include <stb/include/stb_image.h>
#include <stb/include/stb_image_write.h>
#include <tiny_gltf.h>

const std::string APP_NAME = "gltfTest";

#ifdef CHECK
#undef CHECK
#endif

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

using namespace tinygltf;
using juce::File;
using sabi::MeshBuffersHandle;
using sabi::MeshBuffers;
using sabi::Surface;
using sabi::VertexMapRef;
using sabi::VertexMap;
using sabi::VertexMapType;

const std::string resourceFolder = getResourcePath(APP_NAME).toStdString();
const std::string path = resourceFolder + "/Cube/glTF/Cube.gltf";

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

tinygltf::Buffer buf;

inline std::string componentTypeToString(int source)
{
	switch (source)
	{
		case TINYGLTF_COMPONENT_TYPE_BYTE: return "BYTE";
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: return "UNSIGNED BYTE";
		case TINYGLTF_COMPONENT_TYPE_SHORT: return "SHORT";
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: return "UNSIGNED SHORT";
		case TINYGLTF_COMPONENT_TYPE_INT: return "INT";
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: return "UNSIGNED INT";
		case TINYGLTF_COMPONENT_TYPE_FLOAT: return "FLOAT";
		case TINYGLTF_COMPONENT_TYPE_DOUBLE: return "DOUBLE";
		default: return "UNKNOWN TYPE";
	}
}

void processTextureCoords_0(Model& model, const Node& node, const int32_t accessorIndex, MeshBuffersHandle& m)
{
	const auto& accessor = model.accessors[accessorIndex];
	const auto& bufferView = model.bufferViews[accessor.bufferView];
	const auto& buffer = model.buffers[bufferView.buffer];

	//specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
	int componentCount = 1;
	if (accessor.type != TINYGLTF_TYPE_SCALAR)
	{
		componentCount = accessor.type;

		bool sizeOK = componentCount > 0 && componentCount < 5;
		CHECK(sizeOK);
	}

	int uvCount = accessor.count;
	CHECK(uvCount == 36); // 12 triangles with 3 vertices each why 36 vertices instead of 8?

	VertexMapRef vmap = VertexMap::create();
	vmap->dim = componentCount;
	vmap->type = VertexMapType::UV;
	vmap->values.resize(componentCount, uvCount);

	const size_t offset = bufferView.byteOffset;
	std::memcpy(vmap->values.data(), &buffer.data.at(offset), vmap->values.size() * sizeof(float));
	m->vmaps.insert(std::make_pair("UV", vmap));

#if 0
	for (int i = 0; i < uvCount; i++)
	{
		std::cout << vmap->values.col(i).x() << ", " << vmap->values.col(i).y() << std::endl;
	}
#endif


}

void processNodeMormals(Model& model, const Node& node, const int32_t accessorIndex, MeshBuffersHandle& m)
{
	const auto& accessor = model.accessors[accessorIndex];
	const auto& bufferView = model.bufferViews[accessor.bufferView];
	const auto& buffer = model.buffers[bufferView.buffer];

	//specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
	int componentCount = 1;
	if (accessor.type != TINYGLTF_TYPE_SCALAR)
	{
		componentCount = accessor.type;

		bool sizeOK = componentCount > 0 && componentCount < 5;
		CHECK(sizeOK);
	}

	int nCount = accessor.count;
	CHECK(nCount == 36); // 1 for each vertex

	m->N.resize(componentCount, nCount);
	const size_t offset = bufferView.byteOffset;
	std::memcpy(m->N.data(), &buffer.data.at(offset), m->N.size() * sizeof(float));

	

#if 0
	for (int i = 0; i < nCount; i++)
	{
		std::cout << m->N.col(i).x() << ", " << m->N.col(i).y()  << ", " <<  m->N.col(i).z() <<  std::endl;
	}
#endif
}

void processNodeVertices(Model& model, const Node& node, const int32_t accessorIndex, MeshBuffersHandle& m)
{
	const auto& accessor = model.accessors[accessorIndex];
	const auto& bufferView = model.bufferViews[accessor.bufferView];
	const auto& buffer = model.buffers[bufferView.buffer];

	//specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
	int componentCount = 1;
	if (accessor.type != TINYGLTF_TYPE_SCALAR)
	{
		componentCount = accessor.type;

		bool sizeOK = componentCount > 0 && componentCount < 5;
		CHECK(sizeOK);
	}

	int vertexCount = accessor.count;
	CHECK(vertexCount == 36); // 12 triangles with 3 vertices each why 36 vertices instead of 8?
	
	m->V.resize(componentCount, vertexCount);
	const size_t offset = bufferView.byteOffset;
	std::memcpy(m->V.data(), &buffer.data.at(offset), m->V.size() * sizeof(float));

#if 0
	for (int i = 0; i < vertexCount; i++)
	{
		std::cout << m->V.col(i).x() << ", " << m->V.col(i).y() << ", " << m->V.col(i).z() << std::endl;
	}
#endif
}

void processNodeIndices(Model& model, const Node& node, const int32_t accessorIndex, MeshBuffersHandle & m)
{
	const auto& accessor = model.accessors[accessorIndex];
	const auto& bufferView = model.bufferViews[accessor.bufferView];
	const auto& buffer = model.buffers[bufferView.buffer];

	uint32_t numberOfIndices = static_cast<uint32_t>(accessor.count);
	CHECK(numberOfIndices == 36);

	const int32_t sizeofElement =
		accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? 2 :
		accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT ? 4 :
		accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT ? 4 :
		0;

	int componentCount = 3;

	// why is the accessor type for indices a scalar instead of a vector?
	if (accessor.type != TINYGLTF_TYPE_SCALAR)
	{
		componentCount = accessor.type;

		bool sizeOK = componentCount > 0 && componentCount < 5;
		CHECK(sizeOK);
	}

	int triangleCount = numberOfIndices / componentCount;
	CHECK(triangleCount == 12);

	MatrixXus F;
	F.resize(componentCount, triangleCount);
	std::memcpy(F.data(), buffer.data.data(), F.size() * sizeofElement);

	Surface s;
	s.indices() = F.cast<uint32_t>();
	m->S.push_back(s);

#if 0
	for (int i = 0; i < triangleCount; i++)
	{
		std::cout << F.col(i).x() << ", " << F.col(i).y() << ", " << F.col(i).z() << std::endl;
	}
#endif

}

void processNodes(Model& model)
{
	std::vector<int32_t> root_nodes(model.nodes.size(), 1);
	for (auto& node : model.nodes)
	{

		for (int32_t child : node.children)
		{

			root_nodes[child] = 0;
		}
	}

	for (size_t i = 0; i < root_nodes.size(); ++i)
	{
		if (!root_nodes[i])
			continue;

		auto& node = model.nodes[i];
		std::cout << node.name << std::endl;

		const auto& gltf_mesh = model.meshes[node.mesh];
		std::cerr << "Processing glTF mesh: '" << gltf_mesh.name << "'\n";
		std::cerr << "\tNum mesh primitive groups: " << gltf_mesh.primitives.size() << std::endl;
		for (auto& primitive : gltf_mesh.primitives)
		{
			if (primitive.mode != TINYGLTF_MODE_TRIANGLES) // Ignore non-triangle meshes
			{
				std::cerr << "\tNon-triangle primitive: skipping\n";
				continue;
			}
			

			MeshBuffersHandle m = std::make_shared<MeshBuffers>();
			processNodeIndices(model, node, primitive.indices, m);

			for (auto& attrib : primitive.attributes)
			{
				if (attrib.first.compare("POSITION") == 0)
				{
					processNodeVertices(model, node, attrib.second, m);
				}

				if (attrib.first.compare("NORMAL") == 0)
				{
					processNodeMormals(model, node, attrib.second, m);
				}

				if (attrib.first.compare("TEXCOORD_0") == 0)
				{
					processTextureCoords_0(model, node, attrib.second, m);
				}
			}
		}
	}
}

void processImages(Model& model)
{
	for (const auto& gltf_image : model.images)
	{
		std::cout << "Processing image '" << gltf_image.name << "'\n"
			<< "\t(" << gltf_image.width << "x" << gltf_image.height << ")x" << gltf_image.component << "\n"
			<< "\tbits: " << gltf_image.bits << std::endl;

		CHECK(gltf_image.component == 4);
		bool checkBits = gltf_image.bits == 8 || gltf_image.bits == 16;
		CHECK(checkBits);

	/*	scene.addImage(
			gltf_image.width,
			gltf_image.height,
			gltf_image.bits,
			gltf_image.component,
			gltf_image.image.data()
		);*/
	}
}

void processBuffers(Model& model)
{
	for (const auto& gltf_buffer : model.buffers)
	{
		buf = gltf_buffer;
		const uint64_t buf_size = gltf_buffer.data.size();
		std::cout << "Processing glTF buffer '" << gltf_buffer.name << "'\n"
			<< "\tbyte size: " << buf_size << "\n"
			<< "\turi      : " << gltf_buffer.uri << std::endl;

		//scene.addBuffer(buf_size, gltf_buffer.data.data());
	}
}


bool loadModel(Model& model, const char* filename)
{
	TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << filename << std::endl;
	else
		std::cout << "Loaded glTF: " << filename << std::endl;

	return res;
}

TEST_CASE("[load] testing gltfLoad")
{
	File f(path);
	if (!f.existsAsFile())
		throw std::runtime_error("File not found: " + path);

	Model model;
	CHECK(loadModel(model, path.c_str()));

	processBuffers(model);
	processImages(model);
	processNodes(model);
}


class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings = DesktopWindowSettings(), bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
		try
		{
			doctest::Context().run();

		}
		catch (std::exception& e)
		{
			LOG(CRITICAL) << e.what();
		}
	}

  private:
	
};

Jahley::App* Jahley::CreateApplication()
{
	return new Application();
}


#if 0
void bindMesh(Model& model, Mesh& mesh)
{
	for (size_t i = 0; i < model.bufferViews.size(); ++i)
	{
		std::cout << "--------------- Buffer " << i << std::endl;
		const BufferView& bufferView = model.bufferViews[i];

		CHECK(bufferView.target != 0);

		if (bufferView.target == 0)
		{  // TODO impl drawarrays
			std::cout << "WARN: bufferView.target is zero" << std::endl;
			continue;  // Unsupported bufferView.
					   /*
						 From spec2.0 readme:
						 https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
								  ... drawArrays function should be used with a count equal to
						 the count            property of any of the accessors referenced by the
						 attributes            property            (they are all equal for a given
						 primitive).
					   */
		}

		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
		if (bufferView.target == 34962)
		{
			std::cout << "bufferView.target = ARRAY_BUFFER" << std::endl;
		}
		else if (bufferView.target == 34963)
		{
			std::cout << "bufferView.target = ELEMENT_ARRAY_BUFFER" << std::endl;
		}

		std::cout << "buffer.data.size = " << buffer.data.size()
			<< ", bufferview.byteOffset = " << bufferView.byteOffset
			<< ", bufferview.byteLength = " << bufferView.byteLength
			<< std::endl;
	}

	CHECK(mesh.primitives.size() > 0);

	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		Primitive primitive = mesh.primitives[i];


		{

			const tinygltf::Accessor& indexAccessor =
				model.accessors[primitive.indices];

			std::cout << "indexaccessor: count " << indexAccessor.count << ", type "
				<< componenTypeToString(indexAccessor.componentType) << std::endl;

			std::cout << "indices : " << primitive.indices << std::endl;
			std::cout << "mode     : "
				<< "(" << primitive.mode << ")" << std::endl;
		}
#if 0
		std::cout << "-----------------INDICIES-----------------------" << std::endl;
		Accessor indexAccessor = model.accessors[primitive.indices];
		const BufferView& bufferView = model.bufferViews[indexAccessor.bufferView];
		Buffer& buffer = model.buffers[bufferView.buffer];

		std::cout << componenTypeToString(indexAccessor.componentType) << std::endl;
		std::cout << "count: " << indexAccessor.count << std::endl;

		int byteStride = indexAccessor.ByteStride(model.bufferViews[indexAccessor.bufferView]);
		std::cout << "Byte stride: " << byteStride << std::endl;

		int rows = 3; // indexAccessor.type;
		int cols = indexAccessor.count;

		MatrixXu indices;
		indices.resize(rows, cols);
		const size_t offset = bufferView.byteOffset;
		std::memcpy(indices.data(), &buffer.data.at(offset), indices.size() * sizeof(float));

		for (int i = 0; i < cols; i++)
		{
			std::cout << indices.col(i).x() << ", " << indices.col(i).y() << ", " << indices.col(i).z() << std::endl;
		}

#endif
		continue;
		for (auto& attrib : primitive.attributes)
		{
			Accessor accessor = model.accessors[attrib.second];
			int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
			std::cout << "----------------------------------------" << std::endl;
			const BufferView& bufferView = model.bufferViews[accessor.bufferView];
			Buffer& buffer = model.buffers[bufferView.buffer];

			//specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4.
			int nComponents = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR)
			{
				nComponents = accessor.type;

				bool sizeOK = nComponents > 0 && nComponents < 5;
				CHECK(sizeOK);
			}

			std::cout << "Number of components: " << nComponents << std::endl;


			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0)
			{
				std::cout << "Byte stride: " << byteStride << std::endl;
				std::cout << "Buffer view index: " << accessor.bufferView << std::endl;
				std::cout << "POSITION" << std::endl;
				std::cout << componenTypeToString(accessor.componentType) << std::endl;
				vaa = 0;

				std::cout << "COUNT: " << accessor.count << std::endl;

				int rows = nComponents;
				int cols = accessor.count;
				CHECK(cols == 36); // 12 triangles with 3 vertices each

				MatrixXf N;
				N.resize(rows, cols);
				const size_t offset = bufferView.byteOffset;
				std::memcpy(N.data(), &buffer.data.at(offset), N.size() * sizeof(float));

				for (int i = 0; i < cols; i++)
				{
					std::cout << N.col(i).x() << ", " << N.col(i).y() << ", " << N.col(i).z() << std::endl;
				}


			}
			if (attrib.first.compare("NORMAL") == 0)
			{
				std::cout << "COUNT: " << accessor.count << std::endl;
				std::cout << "Byte stride: " << byteStride << std::endl;
				std::cout << "Buffer view index: " << accessor.bufferView << std::endl;
				std::cout << "NORMAL" << std::endl;
				std::cout << componenTypeToString(accessor.componentType) << std::endl;
				vaa = 1;

				int rows = nComponents;
				int cols = accessor.count;
				CHECK(cols == 36);

				MatrixXf N;
				N.resize(rows, cols);
				const size_t offset = bufferView.byteOffset;
				std::memcpy(N.data(), &buffer.data.at(offset), N.size() * sizeof(float));

				for (int i = 0; i < cols; i++)
				{
					std::cout << N.col(i).x() << ", " << N.col(i).y() << ", " << N.col(i).z() << std::endl;
				}
			}
			if (attrib.first.compare("TEXCOORD_0") == 0)
			{
				std::cout << "COUNT: " << accessor.count << std::endl;
				std::cout << "Byte stride: " << byteStride << std::endl;
				std::cout << "Buffer view index: " << accessor.bufferView << std::endl;
				std::cout << "TEXCOORD_0" << std::endl;
				std::cout << componenTypeToString(accessor.componentType) << std::endl;
				vaa = 2;

				int rows = nComponents;
				int cols = accessor.count;
				CHECK(cols == 36);

				MatrixXf N;
				N.resize(rows, cols);
				const size_t offset = bufferView.byteOffset;
				std::memcpy(N.data(), &buffer.data.at(offset), N.size() * sizeof(float));

				for (int i = 0; i < cols; i++)
				{
					std::cout << N.col(i).x() << ", " << N.col(i).y() << std::endl;
				}
			}
			if (vaa > -1)
			{


				//BUFFER_OFFSET

				std::cout << "glVertexAttribPointer" << std::endl;
				/*glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride, BUFFER_OFFSET(accessor.byteOffset));*/
			}
			else
			{
				std::cout << "Byte stride: " << byteStride << std::endl;
				std::cout << "Buffer view index: " << accessor.bufferView << std::endl;
				std::cout << "vaa missing: " << attrib.first << std::endl;
			}

		}
	}
}


void bindModelNodes(tinygltf::Model& model, tinygltf::Node& node)
{
	bindMesh(model, model.meshes[node.mesh]);
	for (size_t i = 0; i < node.children.size(); i++)
	{
		bindModelNodes(model, model.nodes[node.children[i]]);
	}
}

void bindModel(Model& model)
{
	const tinygltf::Scene& scene = model.scenes[model.defaultScene];

	CHECK(scene.nodes.size() > 0);

	for (size_t i = 0; i < scene.nodes.size(); ++i)
	{
		bindModelNodes(model, model.nodes[scene.nodes[i]]);
	}
}
#endif