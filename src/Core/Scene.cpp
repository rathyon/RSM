#include "Scene.h"

#include "Model.h"

using namespace rsm;

Scene::Scene() { }

void Scene::addModel(const sref<Model>& model) {
	_models.push_back(model);
}

const std::vector<sref<Model>>& Scene::models() const {
	return _models;
}