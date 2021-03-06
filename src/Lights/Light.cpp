#include "Light.h"

using namespace rsm;

Light::Light() : SceneObject(), _on(true), _emission(glm::vec3(1.0f)) { }

Light::Light(const glm::vec3& emission)
	: SceneObject(), _on(true), _emission(emission) { }

Light::Light(const glm::vec3& emission, const glm::vec3& position)
	: SceneObject(position), _on(true), _emission(emission) { }

bool Light::isOn() const {
	return _on;
}

bool Light::castingShadows() const {
	return _shadows;
}

glm::vec3 Light::emission() const {
	return _emission;
}

void Light::setOn(bool on) {
	_on = on;
}

void Light::setCastShadows(bool val) {
	_shadows = val;
}

void Light::setEmission(glm::vec3& emission) {
	_emission = emission;
}

glm::mat4 Light::projMatrix() const {
	return _projMatrix;
}

int Light::gBufferWidth() const {
	return _gBufferWidth;
}

int Light::gBufferHeight() const {
	return _gBufferHeight;
}

GLuint Light::gBuffer() const {
	return _gBuffer;
}

GLuint Light::depthMap() const {
	return _depthMap;
}

GLuint Light::positionMap() const {
	return _positionMap;
}

GLuint Light::normalMap() const {
	return _normalMap;
}

GLuint Light::fluxMap() const {
	return _fluxMap;
}