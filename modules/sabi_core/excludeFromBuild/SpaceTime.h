// This header file was auto-generated by ClassMate++
// Created: 16 Sep 2017 5:37:16 pm
// Copyright (c) 2017, HurleyWorks

#pragma once

using Eigen::Affine3f;
using Eigen::AlignedBox3f;
using Eigen::Vector3f;

class SpaceTime 
{

 public:
	SpaceTime ();
	~SpaceTime ();

	void reset();
		
	Affine3f startTransform;
	Affine3f localTransform;
	Affine3f worldTransform;
	Affine3f previousWorldTransform;

	AlignedBox3f modelBound;
	AlignedBox3f worldBound;

	Vector3f scale;
	Vector3f previousScale;

	Vector3f modeledOffset;

	std::chrono::time_point<std::chrono::system_clock> birth;
	std::chrono::duration<double> getLifeSpan();

	bool is2D() const;
	bool hasModeledOffset() const { return modeledOffset.norm() > std::numeric_limits<float>::epsilon(); }
	void updateWorldBounds(bool includeScale = false);
	void updateLocalBounds(bool includeScale = false);
	void makeCurrentPoseStartPose() { startTransform = worldTransform; }

	bool convertToRightHandCoordinateSystem = false;

	void debug()
	{
		matStr4f(startTransform, DBUG, "Start transform");
		matStr4f(localTransform, DBUG, "Local transform");
		matStr4f(worldTransform, DBUG, "World transform");
		matStr4f(previousWorldTransform, DBUG, "Previous world transform");

		vecStr3f(scale, DBUG,  "Scale");
		vecStr3f(previousScale, DBUG, "Previous scale");

		vecStr3f(modelBound.min(), DBUG, "ModelBound min");
		vecStr3f(modelBound.max(), DBUG, "ModelBound max");

		vecStr3f(worldBound.min(), DBUG, "WorldBound min");
		vecStr3f(worldBound.max(), DBUG, "WorlddBound max");

		vecStr3f(modeledOffset, DBUG, "Modeled offset");
	}

}; // end class SpaceTime
