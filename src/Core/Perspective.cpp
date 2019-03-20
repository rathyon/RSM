#include "Perspective.h"

using namespace rsm;

Perspective::Perspective() { }

Perspective::Perspective(int width, int height,
	glm::vec3& eye, glm::vec3& at, glm::vec3& up,
	float near, float far, float fov)

	: Camera(width, height, eye, at, up, near, far), _fov(fov) {

	_projMatrix = glm::perspectiveFov(fov, (float)width, (float)height, near, far);
}

float Perspective::fov() const {
	return _fov;
}