#include "Resources.h"

using namespace rsm;

Resources::Resources() { }
Resources::~Resources() { }

Resources& Resources::get() {
	static Resources _inst;
	return _inst;
}

void Resources::init() {

}

void Resources::addMesh(const std::string& name, const sref<Mesh>& mesh) {
	_meshes[name] = mesh;
}

void Resources::addModel(const std::string& name, const sref<Model>& model) {
	_models[name] = model;
}

void Resources::addShader(const std::string& name, const sref<Shader>& shader) {
	_shaders[name] = shader;
}

void Resources::addTexture(const std::string& name, const sref<Texture>& texture) {
	_textures[name] = texture;
}

void Resources::addMaterial(const std::string& name, const sref<Material>& material) {
	_materials[name] = material;
}

bool Resources::removeMesh(const std::string& name) {
	auto it = _meshes.find(name);
	if (it != _meshes.end()) {
		_meshes.erase(name);
		return true;
	}
	return false;
}

bool Resources::removeModel(const std::string& name) {
	auto it = _models.find(name);
	if (it != _models.end()) {
		_models.erase(name);
		return true;
	}
	return false;
}

bool Resources::removeShader(const std::string& name) {
	auto it = _shaders.find(name);
	if (it != _shaders.end()) {
		_shaders.erase(name);
		return true;
	}
	return false;
}

bool Resources::removeTexture(const std::string& name) {
	auto it = _textures.find(name);
	if (it != _textures.end()) {
		_textures.erase(name);
		return true;
	}
	return false;
}

bool Resources::removeMaterial(const std::string& name) {
	auto it = _materials.find(name);
	if (it != _materials.end()) {
		_materials.erase(name);
		return true;
	}
	return false;
}

sref<Mesh> Resources::getMesh(const std::string& name) {
	//return _meshes.at(name).get();
	return _meshes.at(name);
}

sref<Model> Resources::getModel(const std::string& name) {
	//return _models.at(name).get();
	return _models.at(name);
}

sref<Shader> Resources::getShader(const std::string& name) {
	//return _shaders.at(name).get();
	return _shaders.at(name);
}

sref<Texture> Resources::getTexture(const std::string& name) {
	return _textures.at(name);
}

sref<Material> Resources::getMaterial(const std::string& name) {
	return _materials.at(name);
}

void Resources::cleanup() {
	_shaders.clear();
}