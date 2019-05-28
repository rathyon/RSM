#include "SpotLight.h"

using namespace rsm;

SpotLight::SpotLight() : Light() { }

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff)
	: Light(emission, intensity) {

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;

	_direction = glm::vec3(0.0f, -1.0f, 0.0f);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff, const glm::vec3& direction)
	: Light(emission, intensity) { 

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;

	_direction = glm::normalize(direction);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff, const glm::vec3& direction, const glm::vec3& position)
	: Light(emission, intensity, position) {

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;

	_direction = glm::normalize(direction);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff, const glm::vec3& direction, const glm::vec3& position, float linearAttenuation, float quadraticAttenuation) {
	// approximate values for MOST cases
	_linearAttenuation = linearAttenuation;
	_quadraticAttenuation = quadraticAttenuation;

	_direction = glm::normalize(direction);
	_cutoff = glm::cos(glm::radians(cutoff));
}

glm::vec3 SpotLight::direction() const {
	return _direction;
}

float SpotLight::cutoff() const {
	return _cutoff;
}

void SpotLight::toData(LightData& data) const {
	data.position = _position;
	data.direction = _direction;
	data.emission = _emission;
	data.intensity = _intensity;
	data.linear = _linearAttenuation;
	data.quadratic = _quadraticAttenuation;
	data.type = LightType::LIGHTYPE_SPOT;
	data.state = _on;
	data.cutoff = _cutoff;
}

GLenum SpotLight::depthMapType() {
	return OpenGLTexTargets[IMG_2D];
}

void SpotLight::prepare(int resolution) {
	_resolution = resolution;

	_projMatrix = glm::perspective(glm::acos(_cutoff), 1.0f, 0.1f, 1000.0f);
	_viewMatrix = glm::lookAt(_position,
		_position + _direction,
		glm::vec3(0.0f, 1.0f, 0.0f));
	_viewProjMatrix = _projMatrix * _viewMatrix;

	// prepare framebuffer and shadow map (texture)
	glGenFramebuffers(1, &_depthMapFBO);

	glGenTextures(1, &_depthMap);
	glBindTexture(OpenGLTexTargets[IMG_2D], _depthMap);
	glTexImage2D(OpenGLTexTargets[IMG_2D], 0, GL_DEPTH_COMPONENT32F, _resolution, _resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(OpenGLTexTargets[IMG_2D], GL_TEXTURE_WRAP_S, OpenGLTexWrapping[CLAMP_BORDER]);
	glTexParameteri(OpenGLTexTargets[IMG_2D], GL_TEXTURE_WRAP_T, OpenGLTexWrapping[CLAMP_BORDER]);
	glTexParameteri(OpenGLTexTargets[IMG_2D], GL_TEXTURE_MIN_FILTER, OpenGLTexFilters[NEAREST]);
	glTexParameteri(OpenGLTexTargets[IMG_2D], GL_TEXTURE_MAG_FILTER, OpenGLTexFilters[NEAREST]);

	// guarantee borders are white (zones outside of frustrum must not be in shadow)
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
	glDrawBuffers(0, GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	checkOpenGLError("Error in preparing light source!");
}

void SpotLight::uploadSpatialData(GLuint program) {
	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(_viewProjMatrix));
}

void SpotLight::uploadShadowMapData(GLuint program) {
	uploadSpatialData(program);
}