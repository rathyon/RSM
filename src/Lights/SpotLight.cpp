#include "SpotLight.h"

using namespace rsm;

SpotLight::SpotLight() : Light() { }

SpotLight::SpotLight(const glm::vec3& emission, float cutoff)
	: Light(emission) {

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;

	_direction = glm::vec3(0.0f, -1.0f, 0.0f);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float cutoff, const glm::vec3& direction)
	: Light(emission) { 

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;

	_direction = glm::normalize(direction);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float cutoff, const glm::vec3& direction, const glm::vec3& position)
	: Light(emission, position) {

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;

	_direction = glm::normalize(direction);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float cutoff, const glm::vec3& direction, const glm::vec3& position, float linearAttenuation, float quadraticAttenuation)
	: Light(emission, position) {
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
	data.linear = _linearAttenuation;
	data.quadratic = _quadraticAttenuation;
	data.type = LightType::LIGHTYPE_SPOT;
	data.state = _on;
	data.cutoff = _cutoff;
}

GLenum SpotLight::depthMapType() {
	return OpenGLTexTargets[IMG_2D];
}

void SpotLight::prepare(int width, int height) {
	_gBufferWidth = width;
	_gBufferHeight = height;

	_projMatrix = glm::perspective(glm::acos(_cutoff), (float) width / (float) height, 0.1f, 10000.0f);
	_viewMatrix = glm::lookAt(_position,
		_position + _direction,
		glm::vec3(0.0f, 1.0f, 0.0f));
	_viewProjMatrix = _projMatrix * _viewMatrix;

	// prepare gbuffer framebuffer object
	glGenFramebuffers(1, &_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

	// depth buffer
	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_2D, _depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _gBufferWidth, _gBufferHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// guarantee borders are white (zones outside of frustrum must not be in shadow)
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
	glDrawBuffers(0, GL_NONE);

	// world space coordinates / position buffer
	glGenTextures(1, &_positionMap);
	glBindTexture(GL_TEXTURE_2D, _positionMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _positionMap, 0);

	// normals buffer
	glGenTextures(1, &_normalMap);
	glBindTexture(GL_TEXTURE_2D, _normalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _normalMap, 0);

	// flux buffer
	glGenTextures(1, &_fluxMap);
	glBindTexture(GL_TEXTURE_2D, _fluxMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _fluxMap, 0);

	GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOGE("Framebuffer not complete!\n");

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkOpenGLError("Error in preparing spotlight!");

}

void SpotLight::uploadSpatialData(GLuint program) {
	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(_viewProjMatrix));
}

void SpotLight::uploadShadowMapData(GLuint program) {
	uploadSpatialData(program);
}

void SpotLight::updateMatrix() {
	_projMatrix = glm::perspective(glm::acos(_cutoff), 1.0f, 0.1f, 1000.0f);
	_viewMatrix = glm::lookAt(_position,
		_position + _direction,
		glm::vec3(0.0f, 1.0f, 0.0f));
	_viewProjMatrix = _projMatrix * _viewMatrix;
}