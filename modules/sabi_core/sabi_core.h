#pragma once

#include "../wabi_core/wabi_core.h"

const Eigen::Vector3f DEFAULT_CAMERA_POSIIION = Eigen::Vector3f(0.0f, 4.0f, -10.0f);
const Eigen::Vector3f DEFAULT_CAMERA_TARGET = Eigen::Vector3f::Zero();
const float DEFAULT_FOV_DEGREES = 45.0f;
const float DEFAULT_ASPECT = (float)DEFAULT_DESKTOP_WINDOW_WIDTH / (float)DEFAULT_DESKTOP_WINDOW_HEIGHT;

// mapped from GLFW  // FIXME there must be a better place for this
#define MOUSE_BUTTON_1         0
#define MOUSE_BUTTON_2         1
#define MOUSE_BUTTON_3         2
#define MOUSE_BUTTON_4         3
#define MOUSE_BUTTON_5         4
#define MOUSE_BUTTON_6         5
#define MOUSE_BUTTON_7         6
#define MOUSE_BUTTON_8         7
#define MOUSE_BUTTON_LAST      MOUSE_BUTTON_8
#define MOUSE_BUTTON_LEFT      MOUSE_BUTTON_1
#define MOUSE_BUTTON_RIGHT     MOUSE_BUTTON_2
#define MOUSE_BUTTON_MIDDLE    MOUSE_BUTTON_3

#define MOUSE_RELEASE                0
#define MOUSE_PRESS                  1
#define MOUSE_REPEAT                 2

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

#include "excludeFromBuild/properties/ImageProps.h"
#include "excludeFromBuild/properties/WorldProps.h"
#include "excludeFromBuild/properties/PropertiesServices.h"