#include "Perspective.h"

using namespace rsm;

Perspective::Perspective() { }

Perspective::Perspective(int width, int height,
	glm::vec3 eye, glm::vec3 at, glm::vec3 up,
	float n, float f, float fov)

	: Camera(width, height, eye, at, up, n, f), _fov(fov) {

	_projMatrix = glm::perspective(glm::radians(fov), aspect(), n, f);
	//_projMatrix = glm::perspectiveFov(fov, (float)width, (float)height, n, f);
}

// expressed in degrees
float Perspective::fov() const {
	return _fov;
}

void Perspective::updateProjMatrix(int width, int height) {
	Camera::updateProjMatrix(width, height);
	_projMatrix = glm::perspective(glm::radians(_fov), aspect(), _near, _far);
}