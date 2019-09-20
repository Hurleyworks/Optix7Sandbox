// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.

#include "berserkpch.h"
#include "sabi_core.h"

#define TINYGLTF_IMPLEMENTATION

#include <tiny_gltf.h>

namespace sabi
{
	#include "excludeFromBuild/SpaceTime.cpp"
	#include "excludeFromBuild/InputEvent.cpp"
	#include "excludeFromBuild/ActiveCam.cpp"
	#include "excludeFromBuild/Surface.cpp"
	#include "excludeFromBuild/MeshOps.cpp"
	#include "excludeFromBuild/NormalizedClump.cpp"
	#include "excludeFromBuild/WorldItem.cpp"
	#include "excludeFromBuild/WorldComposite.cpp"
	#include "excludeFromBuild/VertexMap.cpp"
	#include "excludeFromBuild/glTFHandler.cpp"
}