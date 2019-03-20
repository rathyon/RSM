#include "Scene.h"

using namespace rsm;

Scene::Scene() { }

void Scene::addCamera(const sref<Camera>& camera) {
	_cameras.push_back(camera);
}

void Scene::addLight(const sref<Light>& light) {
	_lights.push_back(light);
}

void Scene::addModel(const sref<Model>& model) {
	_models.push_back(model);
}

const std::vector<sref<Camera>>& Scene::cameras() const {
	return _cameras;
}

const std::vector<sref<Light>>& Scene::lights() const {
	return _lights;
}

const std::vector<sref<Model>>& Scene::models() const {
	return _models;
}

void Scene::draw() {

	for (sref<Model> model : _models) {
		model->draw();
	}

}