#include "BlinnPhongMaterial.h"

using namespace rsm;

BlinnPhongMaterial::BlinnPhongMaterial() {
	// when final shaders are done, set _prog here!

	_ambient = vec3(0);
	_diffuse = vec3(0);
	_specular = vec3(0);
}

void BlinnPhongMaterial::uploadData() {
	// set shader variables
	setVec3("ambient", _ambient);
	setVec3("diffuse", _diffuse);
	setVec3("specular", _specular);
}

void BlinnPhongMaterial::setAmbient(const vec3& ambient) {
	_ambient = ambient;
}

void BlinnPhongMaterial::setDiffuse(const vec3& diffuse) {
	_diffuse = diffuse;
}

void BlinnPhongMaterial::setSpecular(const vec3& specular) {
	_specular = specular;
}

vec3 BlinnPhongMaterial::ambient() const {
	return _ambient;
}
vec3 BlinnPhongMaterial::diffuse() const {
	return _diffuse;
}
vec3 BlinnPhongMaterial::specular() const {
	return _specular;
}