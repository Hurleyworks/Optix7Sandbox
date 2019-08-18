#pragma once

#include "../wabi_core/wabi_core.h"

const Eigen::Vector3f DEFAULT_CAMERA_POSIIION = Eigen::Vector3f(0.0f, 4.0f, -10.0f);
const Eigen::Vector3f DEFAULT_CAMERA_TARGET = Eigen::Vector3f::Zero();
const float DEFAULT_FOV_DEGREES = 45.0f;
const float DEFAULT_ASPECT = (float)DEFAULT_DESKTOP_WINDOW_WIDTH / (float)DEFAULT_DESKTOP_WINDOW_HEIGHT;

namespace sabi
{
	#include "excludeFromBuild/ActiveCam.h"
	#include "excludeFromBuild/InputEvent.h"

	// renderable
	#include "excludeFromBuild/MeshOptions.h"
	#include "excludeFromBuild/SpaceTime.h"
	#include "excludeFromBuild/Material.h"
	#include "excludeFromBuild/Surface.h"
	#include "excludeFromBuild/MeshBUffers.h"
#include "excludeFromBuild/RenderableDesc.h"
#include "excludeFromBuild/RenderableState.h"
	#include "excludeFromBuild/RenderableData.h"
	
	
	#include "excludeFromBuild/Renderable.h"

	// raw primitive geometry
	#include "excludeFromBuild/primitives/cylinderY.h"
	#include "excludeFromBuild/primitives/capsuleY.h"
	#include "excludeFromBuild/primitives/box.h"
	#include "excludeFromBuild/primitives/ball.h"
	#include "excludeFromBuild/primitives/TorusMesh.h"
	#include "excludeFromBuild/primitives/BunnyMesh.h"
	#include "excludeFromBuild/primitives/SquirtGun.h"

	
}