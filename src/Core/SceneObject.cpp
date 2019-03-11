#include "SceneObject.h"

using namespace rsm;

SceneObject::SceneObject() {
	_position = glm::vec3(0);
	_scale = glm::vec3(1);
	_rotation = glm::quat();
	_parent = nullptr;
}

SceneObject::SceneObject(const glm::vec3& position) {
	_position = position;
	_scale = glm::vec3(1);
	_rotation = glm::quat();
	_parent = nullptr;
}

glm::vec3& SceneObject::position() {
	return _position;
}

glm::vec3& SceneObject::scale() {
	return _scale;
}

glm::quat& SceneObject::rotation() {
	return _rotation;
}

void SceneObject::setPosition(const glm::vec3& position) {
	_position = position;
}

void SceneObject::setScale(const float x, const float y, const float z) {
	_scale = glm::vec3(x, y, z);
}

void SceneObject::setRotation(const glm::quat& rotation) {
	_rotation = rotation;
}

sref<SceneObject> SceneObject::parent() const {
	return _parent;
}