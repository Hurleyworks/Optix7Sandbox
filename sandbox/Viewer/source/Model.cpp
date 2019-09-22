// This source file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks


#include "Model.h"
#include <stb_image.h>
#include <igl/read_triangle_mesh.h>
#include <igl/write_triangle_mesh.h>

using juce::File;
using juce::StringArray;
using juce::String;
using mace::FileServices;
using sabi::InputEvent;
using sabi::Surface;
using sabi::SpaceTime;
using sabi::MeshBuffers;
using sabi::NormalizedClump;
using sabi::MeshOps;
using Eigen::AlignedBox3f;
using Eigen::Vector3f;
using sabi::WorldComposite;
using sabi::PRenderableState;
using sabi::WorldItem;
using igl::read_triangle_mesh;
using igl::write_triangle_mesh;
using namespace std::placeholders;


// ctor
Model::Model(const PropertyService& properties)
	: properties(properties),
	loadMeshCallback(std::bind(&Model::onMeshLoad, this, _1, _2)),
	loadImagesCallback(std::bind(&Model::onImagesLoad, this, _1)),
	world(WorldComposite::create())
{
	loadStrategy = std::make_shared<NormalizedClump>();
	
}

// dtor
Model::~Model()
{
}

void Model::addPrimitive(PrimitiveType type, MeshOptions options)
{
	createGroundPlane(Vector2f(100.0f, 100.0f));
	return;

	MeshBuffersHandle mesh = MeshOps::createPrimitiveMesh(PrimitiveType(type));
	if (!mesh)
	{
		ErrMsg err;
		err.severity = ErrorSeverity::Critical;
		err.message = PrimitiveType(type).toString() + std::string(" primitive creation failed!");
		errorQueue.push(err);
		return;
	}

	// must compute the face count! // FIXME make this automatic so the user doesn't forget
	MatrixXu triangles;
	mesh->getAllSurfaceIndices(triangles);

	LOG(DBUG) << PrimitiveType(type).toString() << " has " << triangles.size() << " triangles and " << mesh->V.cols() << " vertices";

	AlignedBox3f modelBound;
	modelBound.min() = mesh->V.rowwise().minCoeff();
	modelBound.max() = mesh->V.rowwise().maxCoeff();
	float s = 1.0f;

	// this might change scale!
	if ((options & MeshOptions::NormalizeSize) == MeshOptions::NormalizeSize)
	{
		MeshOps::normalizeSize(mesh, modelBound, s);
	}

	if ((options & MeshOptions::CenterVertices) == MeshOptions::CenterVertices)
	{
		MeshOps::centerVertices(mesh, modelBound, s);
	}

	SpaceTime spacetime;
	spacetime.modelBound.min() = mesh->V.rowwise().minCoeff();
	spacetime.modelBound.max() = mesh->V.rowwise().maxCoeff();
	spacetime.worldTransform = Pose::Identity();
	spacetime.scale = Scale::Constant(1.0f);

	if ((options & MeshOptions::LoadStrategy) == MeshOptions::LoadStrategy)
	{
		loadStrategy->addNextItem(spacetime);
	}

	if ((options & MeshOptions::RestOnGround) == MeshOptions::RestOnGround)
	{
		spacetime.worldTransform.translation().y() = -spacetime.modelBound.min().y();
	}

	spacetime.startTransform = spacetime.worldTransform;

	spacetime.updateWorldBounds(true);

	RenderableData data;
	data.clientID = INVALID_ID;
	data.desc = RenderableDesc();
	data.mesh = mesh;
	data.name = PrimitiveType(type).toString();
	data.spacetime = spacetime;
	data.state.state |= PRenderableState::Pickable;

	createNewRenderable(data);
}

void Model::createGroundPlane(const Vector2f& size)
{
	std::string name = "Default 2D ground plane";

	MatrixXu F; // faces
	MatrixXf V; // vertices
	MatrixXf N; // vertex normals

	V.resize(3, 4);
	V.col(0) = Vector3f(-size.x(), 0.0f, size.y());
	V.col(1) = Vector3f(size.x(), 0.0f, size.y());
	V.col(2) = Vector3f(size.x(), 0.0f, -size.y());
	V.col(3) = Vector3f(-size.x(), 0.0f, -size.y());

	F.resize(3, 2);
	F.col(0) = Vector3u(0, 1, 2);
	F.col(1) = Vector3u(2, 3, 0);

	N.resize(3, 4);
	N.col(0) = N.col(1) = N.col(2) = N.col(3) = Vector3f::UnitY();

	SpaceTime spacetime;
	spacetime.modelBound.min() = V.rowwise().minCoeff();
	spacetime.modelBound.max() = V.rowwise().maxCoeff();
	spacetime.worldTransform = Pose::Identity();
	spacetime.startTransform = Pose::Identity();
	spacetime.scale = Scale::Constant(1.0f);
	spacetime.updateWorldBounds();

	MeshBuffersHandle m = std::make_shared<MeshBuffers>();
	m->V = V;
	m->N = N;

	Surface s;
	s.indices() = F;
	m->S.push_back(s);

	RenderableData data;
	data.clientID = -1;
	data.desc = RenderableDesc();
	data.mesh = m;
	data.name = name;
	data.spacetime = spacetime;

	createNewRenderable(data);
}

void Model::loadImage(const std::string& path, PixelBuffer& buffer)
{
	int force_channels = 0;

	handleType pixels(stbi_load(path.c_str(), &buffer.spec.width, &buffer.spec.height, &buffer.spec.channels, force_channels), stbi_image_free);
	if (pixels)
	{
		int pixelCount = buffer.getPixelCount();
		int bytes = pixelCount * buffer.spec.channels * sizeof(uint8_t);

		buffer.uint8Pixels.resize(buffer.spec.channels, pixelCount);
		std::memcpy(buffer.uint8Pixels.data(), pixels.get(), bytes);
	}
}

void Model::onDrop(const std::vector<std::string>& fileList)
{
	for (auto p : fileList)
	{
		File f(p);
		if (f.isDirectory())
		{
			// FIXME
			StringArray files;
			String wildCard("*.gltf;*.ply;*.obj");
			FileServices::getFiles(f.getFullPathName(), files, wildCard);

			for (auto path : files)
			{
				File f(path);
				// load on another thread 
				if (f.getFileExtension() == ".gltf")
				{
					glTFLoader.call(&glTFLoader::loadGeometry, path.toStdString(), loadMeshCallback);
				}
				else
				{
					activeLoader.call(&ActiveLoader::loadMesh, path.toStdString(), loadMeshCallback);
				}
			}
		}
		else if (f.existsAsFile())
		{
			std::string path = f.getFullPathName().toStdString();
			File f(path);

			// load on another thread 
			if (f.getFileExtension() == ".gltf")
			{
				glTFLoader.call(&glTFLoader::loadGeometry, path, loadMeshCallback);
			}
			else
			{
				activeLoader.call(&ActiveLoader::loadMesh, path, loadMeshCallback);
			}
		}
	}
}

void Model::createNewRenderable(const RenderableData& d)
{
	RenderableNode node = WorldItem::create();
	node->setClientID(d.clientID);
	node->setDescription(d.desc);
	node->setMesh(d.mesh);
	node->setSpacetime(d.spacetime);
	node->setState(d.state);
	node->setName(d.name);

	// objects are pickable by default
	node->getState().state |= sabi::PRenderableState::Pickable;

	// IMPORTNANT make sure triangle count has been computed
	if (!node->getMesh()->triangleCount())
	{
		MatrixXu allIndices;
		node->getMesh()->getAllSurfaceIndices(allIndices);
	}

	world->addChild(node);

	// update scene stats
	uint64_t totalMeshes = properties.worldProps->getVal<uint64_t>(WorldKey::TotalMeshes);
	properties.worldProps->setValue(WorldKey::TotalMeshes, ++totalMeshes);

	uint64_t totalTriangles = properties.worldProps->getVal<uint64_t>(WorldKey::TotalRealTriangles);
	totalTriangles += d.mesh->triangleCount();
	properties.worldProps->setValue(WorldKey::TotalRealTriangles, totalTriangles);

	node->debug(_FN_);

	emitRenderable(node);
}

void Model::createNewInstance(const RenderableData& data)
{
}

void Model::loadModelFromIcon(const std::string& iconPath)
{
	File f(iconPath);
	String modelName = f.getFileNameWithoutExtension();

	std::string commonFolder = properties.renderProps->getVal<std::string>(RenderKey::CommonFolder);
	std::string modelFolder = commonFolder + "/models";
	StringArray files;
	String wildCard("*.gltf");
	FileServices::getFiles(modelFolder, files, wildCard);

	for (auto path : files)
	{
		File f(path);
		String fileName = f.getFileName();
		if (fileName.startsWithIgnoreCase(modelName))
		{
			LOG(DBUG) << fileName;
			glTFLoader.call(&glTFLoader::loadGeometry, path.toStdString(), loadMeshCallback);
			break;
		}	
	}
}

void Model::addNode(RenderableNode& node)
{
}

void Model::removeNode(RenderableNode& node)
{
}

void Model::removeNode(ItemID itemID)
{
	RenderableNode node = world->findChild(itemID);
	if (node)
	{
		removeNode(node);
	}
	else
	{
		LOG(CRITICAL) << "ItemiD: " << itemID << " is not in the scene graph";
	}
}

void Model::clearScene()
{
	world->removeChildren();
}

void Model::onMeshLoad(MeshBuffersHandle m, std::string path)
{
	// this callback comes from the ActiveLoader thread so
	// we store the mesh in a thread safe container
	// and then pick it up on the main thread during
	// an update()
	meshQueue.enqueue(std::make_pair(m, path));
}

void Model::onImagesLoad(const Images&& images)
{
	// this callback comes from the ActiveLoader thread so
	// we store the images in a thread safe container
	// and then pick it up on the main thread during
	// an update()
	for (auto const& image : images)
	{
		imageQueue.enqueue(image);
	}
}

void Model::saveRender(PixelBuffer& pixelBuffer, const std::string& appName, std::chrono::duration<double> time)
{
	String framegrabPath = properties.renderProps->getVal<std::string>(RenderKey::ResourceFolder);
	framegrabPath += "/frameGrabs";

	File f(framegrabPath);
	if (!f.exists())
	{
		f.createDirectory();
	}

	String name(appName);
	name += "__frameGrab__" + String(time.count()) +  "s.png";

	framegrabPath += File::separator + name;

	// fix file separators
	String path(framegrabPath);
	path = path.replaceCharacter('/', '\\');

	ImageInfo spec = pixelBuffer.spec;
	MatrixXc flipped;
	pixelBuffer.flipVertical(flipped);

	if(!stbi_write_png(path.toStdString().c_str(), spec.width, spec.height, 4, flipped.data(), 4 * spec.width))
	{
		ErrMsg err;
		err.severity = ErrorSeverity::Critical;
		err.message = "Frame saving failed! " + path.toStdString();
		errorQueue.push(err);
	}

}

void Model::addMesh(MeshBuffersHandle mesh,
					const std::string& path,
					BodyID clientID,
					const Pose& pose,
					const Scale& scale,
					RenderableDesc desc,
					MeshOptions options)
{
	MatrixXu tris;
	mesh->getAllSurfaceIndices(tris);

	//MatrixXu F = tris.transpose();
	//MatrixXf V = mesh->V.transpose();
	//write_triangle_mesh(path, V, F);

	AlignedBox3f modelBound;
	modelBound.min() = mesh->V.rowwise().minCoeff();
	modelBound.max() = mesh->V.rowwise().maxCoeff();
	float s = 1.0f;

	// this might change scale!
	if ((options & MeshOptions::NormalizeSize) == MeshOptions::NormalizeSize)
	{
		MeshOps::normalizeSize(mesh, modelBound, s);
	}

	if ((options & MeshOptions::CenterVertices) == MeshOptions::CenterVertices)
	{
		MeshOps::centerVertices(mesh, modelBound, s);
	}

	SpaceTime spacetime;
	spacetime.modelBound.min() = mesh->V.rowwise().minCoeff();
	spacetime.modelBound.max() = mesh->V.rowwise().maxCoeff();
	spacetime.worldTransform = Pose::Identity();
	spacetime.scale = Scale::Constant(1.0f);

	if ((options & MeshOptions::LoadStrategy) == MeshOptions::LoadStrategy)
	{
		loadStrategy->addNextItem(spacetime);
	}

	if ((options & MeshOptions::RestOnGround) == MeshOptions::RestOnGround)
	{
		spacetime.worldTransform.translation().y() = -spacetime.modelBound.min().y();
	}

	spacetime.startTransform = spacetime.worldTransform;

	spacetime.updateWorldBounds(true);

	File f(path);
	desc.fullFilePath = path;

	RenderableData data;
	data.clientID = clientID;
	data.desc = desc;
	data.mesh = mesh;
	data.name = f.getFileNameWithoutExtension().toStdString();
	data.spacetime = spacetime;
	data.state.state |= PRenderableState::Pickable;

	data.debug();

	createNewRenderable(data);
}

