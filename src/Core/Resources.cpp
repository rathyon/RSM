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

void Resources::addShader(const std::string& name, const std::shared_ptr<Shader>& shader) {
	_shaders[name] = shader;
}

bool Resources::removeShader(const std::string& name) {
	auto it = _shaders.find(name);
	if (it != _shaders.end()) {
		_shaders.erase(name);
		return true;
	}
	return false;
}

Shader* Resources::getShader(const std::string& name) {
	return _shaders.at(name).get();
}

void Resources::cleanup() {
	_shaders.clear();
}