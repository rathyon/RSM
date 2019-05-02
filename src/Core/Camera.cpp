#include "Camera.h"

using namespace rsm;

Camera::Camera() { }

Camera::Camera(int width, int height, 
	const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up,
	float n, float f)
	: _width(width), _height(height), _near(n), _far(f), _worldUp(up) {

	lookAt(eye, at, up);

	_front = glm::normalize(at - eye);
	_right = glm::normalize(glm::cross(_worldUp, _front));
	_up = glm::normalize(glm::cross(_front, _right));
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
	return _right;
}

glm::vec3 Camera::front() const {
	return _front;
}

glm::vec3 Camera::up() const {
	return _up;
}

void Camera::lookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up) {
	_position = eye;
	_objToWorld = glm::lookAt(eye, at, up);
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

// check these values
void Camera::updateViewMatrix() {
	glm::vec3 front;

	front.x = cos(_yaw) * cos(_pitch);
	front.y = sin(_pitch);
	front.z = sin(_yaw) * cos(_pitch);

	_front = glm::normalize(front);
	_right = glm::normalize(glm::cross(_front, _worldUp));
	_up = glm::normalize(glm::cross(_right, _front));

	lookAt(_position, _position + _front, _up);
}

void Camera::updateProjMatrix(int width, int height) {
	_width = width;
	_height = height;
}

// check these values
void Camera::updateOrientation(float dpdx, float dydx) {
	_pitch += dpdx;
	_yaw -= dydx;

	_yaw = fmodf(_yaw, glm::two_pi<float>());

	if (_yaw < 0.0f)
		_yaw += glm::two_pi<float>();

	// unsolved issue: when pitch = modulo half PI it "rotates"
	if (_pitch > glm::half_pi<float>())
		_pitch = glm::half_pi<float>();
	else if (_pitch < -glm::half_pi<float>())
		_pitch = -glm::half_pi<float>();
}

void Camera::toData(CameraData& data) {
	data.viewMatrix = viewMatrix();
	data.projMatrix = projMatrix();
	data.viewProjMatrix = viewProjMatrix();
	data.viewPos = position();
}