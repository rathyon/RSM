#include "SceneObject.h"

using namespace rsm;

// TODO create objToWorld matrix from the get-go
// TODO verify quaternion values

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

SceneObject::SceneObject(const glm::mat4& objToWorld) {
	_objToWorld = objToWorld;
	_parent = nullptr;

	_position = glm::vec3(_objToWorld[0][3],
		             _objToWorld[1][3],
					 _objToWorld[2][3]);
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

glm::mat4& SceneObject::objToWorld() {
	return _objToWorld;
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

void SceneObject::setObjToWorld(const glm::mat4& mat) {
	_objToWorld = mat;
}

void SceneObject::updateMatrix() {
	_objToWorld = glm::translate(glm::mat4(1.0f), _position) *
				  glm::mat4_cast(_rotation) *
				  glm::scale(glm::mat4(1.0f), _scale);
}

sref<SceneObject> SceneObject::parent() const {
	return _parent;
}