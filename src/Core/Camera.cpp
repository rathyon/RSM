#include "Camera.h"

using namespace rsm;

Camera::Camera() { }

Camera::Camera(int width, int height, 
	const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up,
	float n, float f)
	: _width(width), _height(height), _near(n), _far(f) {

	lookAt(eye, at, up);

	glm::vec3 viewDir = glm::normalize(at - eye);
	_pitch = asinf(-viewDir.y);
	_yaw = atan2f(viewDir.x, -viewDir.z);
}

int Camera::width() const {
	return _width;
}

int Camera::height() const {
	return _height;
}

float Camera::aspect() const {
	return (float)_width / (float)_height;
}

float Camera::n() const {
	return _near;
}

float Camera::f() const {
	return _far;
}

glm::vec3 Camera::right() const {
	return glm::vec3(_objToWorld[0][0],
					 _objToWorld[0][1],
					 _objToWorld[0][2]);
}

glm::vec3 Camera::front() const {
	return glm::vec3(_objToWorld[2][0],
					 _objToWorld[2][1],
					 _objToWorld[2][2]);
}

glm::vec3 Camera::up() const {
	return glm::vec3(_objToWorld[1][0],
					 _objToWorld[1][1],
					 _objToWorld[1][2]);
}

void Camera::lookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up) {
	_position = eye;
	_objToWorld = glm::lookAt(eye, at, up);

	glm::mat4 matRotation = _objToWorld;
	matRotation[0][3] = 0;
	matRotation[1][3] = 0;
	matRotation[2][3] = 0;
	matRotation[3][3] = 1;
	_rotation = glm::quat_cast(matRotation);
}

void Camera::lookAt(const glm::vec3& at) {
	_objToWorld = glm::lookAt(_position, at, glm::vec3(0.0f, 1.0f, 0.0f));
}

const glm::mat4& Camera::viewMatrix() const {
	return _objToWorld;
}

const glm::mat4& Camera::projMatrix() const {
	return _projMatrix;
}

glm::mat4 Camera::viewProjMatrix() const {
	return _projMatrix * _objToWorld;
}

void Camera::updateViewMatrix() {
	glm::mat4 rotX = glm::rotate(_pitch, glm::vec3(1, 0, 0));
	glm::mat4 rotY = glm::rotate(_yaw, glm::vec3(0, 1, 0));

	glm::mat4 orientation = rotX * rotY;

	_objToWorld = orientation * glm::translate(-_position);
}

void Camera::updateOrientation(float dpdx, float dydx) {
	_pitch += dpdx;
	_yaw -= dydx;

	_yaw = fmodf(_yaw, 2.0f * glm::two_pi<float>());

	if (_yaw < 0.0f)
		_yaw += glm::two_pi<float>();

	if (_pitch > glm::half_pi<float>())
		_pitch = glm::half_pi<float>();
	else if (_pitch < -glm::half_pi<float>())
		_pitch = -glm::half_pi<float>();
}