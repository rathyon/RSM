#include "DirectionalLight.h"

using namespace rsm;

DirectionalLight::DirectionalLight() : Light() { }

DirectionalLight::DirectionalLight(const glm::vec3& emission, float intensity)
	: Light(emission, intensity), _direction(glm::vec3(0.0f, -1.0f, 0.0f)) { }

DirectionalLight::DirectionalLight(const glm::vec3& emission, float intensity, const glm::vec3& direction)
	: Light(emission, intensity), _direction(glm::normalize(direction)) { }

glm::vec3 DirectionalLight::direction() const {
	return _direction;
}

void DirectionalLight::toData(LightData& data) const {
	data.position = _position;
	data.direction = _direction;
	data.emission = _emission;
	data.intensity = _intensity;
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
	_projMatrix = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 1000.0f);
	_viewMatrix = glm::lookAt(_direction * -100.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	_viewProjMatrix = _projMatrix * _viewMatrix;

	// prepare gbuffer framebuffer object
	glGenFramebuffers(1, &_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

	// depth buffer
	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_2D, _depthMap);
	/** /
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _gBufferWidth, _gBufferHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// guarantee borders are white (zones outside of frustrum must not be in shadow)
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(OpenGLTexTargets[IMG_2D], GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
	/**/

	// world space coordinates / position buffer
	glGenTextures(1, &_positionMap);
	glBindTexture(GL_TEXTURE_2D, _positionMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _positionMap, 0);

	// normals buffer
	glGenTextures(1, &_normalMap);
	glBindTexture(GL_TEXTURE_2D, _normalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _normalMap, 0);

	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	//glReadBuffer(GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkOpenGLError("Error in preparing light source!");
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