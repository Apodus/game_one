#include "camera.hpp"

sa::Camera::Camera() {

}

sa::Camera::~Camera() {

}

void sa::Camera::setProjection(float zNear, float zFar, float aspect) {
	float w = zNear;
	float h = zNear / aspect;
	projection.makeFrustumMatrix(
		-w, w,
		-h, h,
		zNear, zFar
	);
}

void sa::Camera::setPosition(const sa::vec3<float>& cameraPosition) {
	this->cameraPosition = cameraPosition;
	sa::vec3<float> center(cameraPosition);
	sa::vec3<float> up(0.f, 1.f, 0.f);
	center.z -= 10;

	view.makeLookAtMatrix(
		cameraPosition.x, cameraPosition.y, cameraPosition.z, 
		center.x, center.y, center.z, 
		up.x, up.y, up.z
	);
}

const sa::Matrix4& sa::Camera::getView() const {
	return view;
}

const sa::Matrix4& sa::Camera::getProjection() const {
	return projection;
}