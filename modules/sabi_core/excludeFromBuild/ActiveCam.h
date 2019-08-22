// This header file was auto-generated by ClassMate++
// Created: 9 May 2019 6:32:48 am
// Copyright (c) 2019, HurleyWorks

#pragma once

using Eigen::Vector3f;
using Eigen::Vector2f;
using Eigen::Vector2i;
using Eigen::Quaternionf;
using Eigen::Matrix4f;
using Eigen::Affine3f;
using Eigen::Translation3f;
using Eigen::AngleAxisf;
using wabi::Ray3f;

using CameraHandle = std::shared_ptr<class ActiveCam>;

class ActiveCam
{
 
 public:
	~ActiveCam ();

	// viewport
	void setScreenSize(const Vector2f & size) { screenSize = size; }
	Vector2f getScreenSize() const { return screenSize; }
	float getScreenWidth() const { return screenSize.x(); }
	float getScreenHeight() const { return screenSize.y(); }

	void lookAt(const Vector3f & eyePoint, const Vector3f & target, const Vector3f & up);
	void lookAt(const Vector3f & eyePoint, const Vector3f & target);
	void setEyePoint(const Vector3f & eyePoint) { eye = eyePoint; viewMatrixCached = false; }
	void setTarget(const Vector3f & target);
	void setDirection(const Vector3f & newDirection);
	void setOrientation(const Matrix3f & linear) { orientation = Quaternionf(linear); viewMatrixCached = false;}

	const Vector3f & getEyePoint() const { return eye; }
	const Vector3f & getWorldUp() const { return worldUp; }
	const Vector3f & getUp() const { return mV; }
	const Vector3f & getRight() const { return mU; }
	const Vector3f & getFoward() const { return forward; }
	const Vector3f & getTarget() const { return target; }
	const Vector3f & getViewDirection() const { return viewDirection; }
	const Quaternionf & getOrientation() const { return orientation; }

	float getFOV() const { return fovDegrees; }
	float getAspect() const { return aspect; }

	void rotateAroundTarget(const Quaternionf& q);
	void zoom(float d);

	const Affine3f & getViewTransform() const { if (!viewMatrixCached) calcViewMatrix(); return viewMatrix; }
	const Matrix4f & getViewMatrix() const { if (!viewMatrixCached) calcViewMatrix(); return viewMatrix.matrix(); }
	const Matrix4f & getInverseViewMatrix() const { if (!inverseModelViewCached) calcInverseView(); return inverseModelViewMatrix; }
	const Matrix4f & getProjectionMatrix() const { if (!projectionCached) calcProjection(); return projectionMatrix; }

	// from Cinder https://libcinder.org/
	Ray3f generateRay(float u, float v, float imagePlaneAspectRatio) const { return calcRay(u, v, imagePlaneAspectRatio); }
	Ray3f generateRay(const Vector2f & posPixels, const Vector2f & imageSizePixels) const
	{
		return calcRay(posPixels.x() / imageSizePixels.x(), (imageSizePixels.y() - posPixels.y()) / imageSizePixels.y(), imageSizePixels.x() / imageSizePixels.y()); 
	}

	void getFrustum(float *left, float *top, float *right, float *bottom, float *nearVal, float *farVal) const;

	virtual void setPerspective(float verticalFovDegrees, float aspectRatio, float nearPlane, float farPlane) {}
	virtual bool isPersp() const = 0;

	bool isDirty() const { return dirty; }
	void setDirty(bool state) { dirty = state; }

	// trackball
	void starTracking() { lastPointOK = false; }
	void track(const Vector2f & newPoint2D);

	// CCD
	void setCCD(const CCD && image){ccd = std::move(image);}
	CCD& getCCD() {return ccd;}

 protected:
	ActiveCam()
		: viewMatrixCached(false),
		  projectionCached(false), 
		  inverseModelViewCached(false),
		  worldUp(Vector3f::UnitY())
	{}

	void calcMatrices() const;
	virtual void calcViewMatrix() const;
	virtual void calcInverseView() const;
	virtual void calcProjection() const = 0;
	virtual wabi::Ray3f	calcRay(float uPos, float vPos, float imagePlaneAspectRatio) const;

	// trackball
	bool mapToSphere(const Vector2f & p2, Vector3f & v3);
	Vector3f lastPoint3D = Vector3f::Zero();
	bool lastPointOK = false;

	bool dirty = true;

	Vector2f screenSize = Vector2f(DEFAULT_DESKTOP_WINDOW_WIDTH, DEFAULT_DESKTOP_WINDOW_HEIGHT);
	Vector3f eye = DEFAULT_CAMERA_POSIIION;
	Vector3f viewDirection = Vector3f::UnitZ();
	mutable Vector3f forward = Vector3f::UnitX();
	Quaternionf	orientation;
	Vector3f worldUp = Vector3f::UnitY();
	Vector3f target = Vector3f::Zero();

	float fovDegrees = DEFAULT_FOV_DEGREES;
	float aspect = DEFAULT_ASPECT;
	float nearClip = 0.01f;
	float farClip = 1000.0f;

	mutable Vector3f mU;	// Right vector
	mutable Vector3f mV;	// Readjust up-vector
	mutable Vector3f mW;	// Negative view direction
	
	mutable Matrix4f projectionMatrix, inverseProjectionMatrix;
	mutable bool projectionCached;
	mutable Affine3f viewMatrix;
	mutable bool viewMatrixCached;
	mutable Matrix4f inverseModelViewMatrix;
	mutable bool inverseModelViewCached;

	mutable float frustumLeft, frustumRight, frustumTop, frustumButtom;

	CCD ccd;

}; // end class ActiveCam


using PerspectiveCamHandle = std::shared_ptr<class PerspectiveCam>;

class PerspectiveCam : public ActiveCam
{
 public:
	static CameraHandle create() { return std::make_shared<PerspectiveCam>(); }

 public:
	PerspectiveCam();
	PerspectiveCam(int pixelWidth, int pixelHeight, float fovDegrees);
	PerspectiveCam(int pixelWidth, int pixelHeight, float fovDegrees, float nearPlane, float farPlane);

	
	void setPerspective(float verticalFovDegrees, float aspectRatio, float nearPlane, float farPlane) override;
	bool isPersp() const override { return true; }

 protected:
	void calcProjection() const override;
};