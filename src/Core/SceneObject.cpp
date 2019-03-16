#include "SceneObject.h"

using namespace rsm;

SceneObject::SceneObject() {
	_position = vec3(0);
	_scale = vec3(1);
	_rotation = quat();
	_parent = nullptr;
}

SceneObject::SceneObject(const vec3& position) {
	_position = position;
	_scale = vec3(1);
	_rotation = quat();
	_parent = nullptr;
}

vec3& SceneObject::position() {
	return _position;
}

vec3& SceneObject::scale() {
	return _scale;
}

quat& SceneObject::rotation() {
	return _rotation;
}

void SceneObject::setPosition(const vec3& position) {
	_position = position;
}

void SceneObject::setScale(const float x, const float y, const float z) {
	_scale = vec3(x, y, z);
}

void SceneObject::setRotation(const quat& rotation) {
	_rotation = rotation;
}

sref<SceneObject> SceneObject::parent() const {
	return _parent;
}