#include "BlinnPhongMaterial.h"

using namespace rsm;

BlinnPhongMaterial::BlinnPhongMaterial() {
	// when final shaders are done, set _prog here!

	_ambient = vec3(0.0f);
	_diffuse = vec3(-1.0f);
	_specular = vec3(-1.0f);
	_shininess = 0.0f;

	_diffuseTex = -1;
	_specularTex = -1;
	_normalMap = -1;
}

void BlinnPhongMaterial::uploadData() {
	// set shader variables
	setVec3("ambient", _ambient);
	setFloat("shininess", _shininess);

	if (_diffuseTex != -1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _diffuseTex);
		setSampler("diffuseTex", 0);
	}
	else {
		setVec3("diffuse", _diffuse);
	}

	if (_specularTex != -1) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _specularTex);
		setSampler("specularTex", 1);
	}
	else {
		setVec3("specular", _specular);
	}

	if (_normalMap != -1) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, _normalMap);
		setSampler("normalMap", 2);
	}

}

void BlinnPhongMaterial::setAmbient(const vec3& ambient) {
	_ambient = ambient;
}

void BlinnPhongMaterial::setDiffuse(const vec3& diffuse) {
	_diffuse = diffuse;
}

void BlinnPhongMaterial::setDiffuseTex(GLuint diffTex) {
	_diffuseTex = diffTex;
}

void BlinnPhongMaterial::setSpecular(const vec3& specular) {
	_specular = specular;
}

void BlinnPhongMaterial::setSpecularTex(GLuint specTex) {
	_specularTex = specTex;
}

void BlinnPhongMaterial::setNormalMap(GLuint normTex) {
	_normalMap = normTex;
}

void BlinnPhongMaterial::setShininess(float shininess) {
	_shininess = shininess;
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

float BlinnPhongMaterial::shininess() const {
	return _shininess;
}

GLuint BlinnPhongMaterial::diffuseTex() const {
	return _diffuseTex;
}
GLuint BlinnPhongMaterial::specularTex() const {
	return _specularTex;
}
GLuint BlinnPhongMaterial::normalMap() const {
	return _normalMap;
}