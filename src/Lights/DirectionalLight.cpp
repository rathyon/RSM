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

void DirectionalLight::prepare(int resolution) {
	_resolution = resolution;

	// shadow mapping for directional lights is weird, they need a "position" for generating the depth map...
	_projMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100000.0f);
	_viewMatrix = glm::lookAt(_direction * -100.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
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

void DirectionalLight::uploadSpatialData(GLuint program) {
	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(_viewProjMatrix));
}

void DirectionalLight::uploadShadowMapData(GLuint program) {
	uploadSpatialData(program);
}