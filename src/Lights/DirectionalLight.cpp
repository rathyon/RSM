#include "DirectionalLight.h"

using namespace rsm;

DirectionalLight::DirectionalLight() : Light() { }

DirectionalLight::DirectionalLight(const glm::vec3& emission)
	: Light(emission), _direction(glm::vec3(0.0f, -1.0f, 0.0f)) { }

DirectionalLight::DirectionalLight(const glm::vec3& emission, const glm::vec3& direction)
	: Light(emission), _direction(glm::normalize(direction)) { }

glm::vec3 DirectionalLight::direction() const {
	return _direction;
}

void DirectionalLight::toData(LightData& data) const {
	data.position = _position;
	data.direction = _direction;
	data.emission = _emission;
	data.linear = 0.0f;
	data.quadratic = 0.0f;
	data.type = LightType::LIGHTYPE_DIR;
	data.state = _on;
	data.cutoff = 0.0f;
}

GLenum DirectionalLight::depthMapType() {
	return OpenGLTexTargets[IMG_2D];
}

void DirectionalLight::prepare(int width, int height) {
	_gBufferWidth = width;
	_gBufferHeight = height;

	// shadow mapping for directional lights is weird, they need a "position" for generating the depth map...
	_projMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 1000.0f);
	_viewMatrix = glm::lookAt(_direction * -10.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	_viewProjMatrix = _projMatrix * _viewMatrix;

	// prepare gbuffer framebuffer object
	glGenFramebuffers(1, &_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

	// depth buffer
	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_2D, _depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, _gBufferWidth, _gBufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// guarantee borders are white (zones outside of frustrum must not be in shadow)
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(OpenGLTexTargets[IMG_2D], GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
	glDrawBuffers(0, GL_NONE);

	// world space coordinates / position buffer
	glGenTextures(1, &_positionMap);
	glBindTexture(GL_TEXTURE_2D, _positionMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _positionMap, 0);

	// normals buffer
	glGenTextures(1, &_normalMap);
	glBindTexture(GL_TEXTURE_2D, _normalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _normalMap, 0);

	// flux buffer
	glGenTextures(1, &_fluxMap);
	glBindTexture(GL_TEXTURE_2D, _fluxMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
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
	checkOpenGLError("Error in preparing directional light source!");
}

void DirectionalLight::uploadSpatialData(GLuint program) {
	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(_viewProjMatrix));
}

void DirectionalLight::uploadShadowMapData(GLuint program) {
	uploadSpatialData(program);
}

void DirectionalLight::updateMatrix() {
	_projMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10000.0f);
	_viewMatrix = glm::lookAt(_direction * -100.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	_viewProjMatrix = _projMatrix * _viewMatrix;
}