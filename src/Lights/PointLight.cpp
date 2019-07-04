#include "PointLight.h"

#ifdef __ANDROID__
#define MOBILE 1
#else
#define MOBILE 0
#endif

using namespace rsm;

PointLight::PointLight() : Light() { }

PointLight::PointLight(const glm::vec3& emission)
	: Light(emission) {
	
	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;
}

PointLight::PointLight(const glm::vec3& emission, const glm::vec3& position)
	: Light(emission, position) {

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;
}

PointLight::PointLight(const glm::vec3& emission, const glm::vec3& position, float linear, float quadratic)
	: Light(emission, position), _linearAttenuation(linear), _quadraticAttenuation(quadratic) { }

void PointLight::toData(LightData& data) const {
	data.position = _position;
	data.direction = glm::vec3(0);
	data.emission = _emission;
	data.linear = _linearAttenuation;
	data.quadratic = _quadraticAttenuation;
	data.type = LightType::LIGHTYPE_POINT;
	data.state = _on;
	data.cutoff = 0.0f;
}

GLenum PointLight::depthMapType() {
	return OpenGLTexTargets[IMG_CUBE];
}

// TODO: add far as argument/parameter
void PointLight::prepare(int width, int height) {
	_gBufferWidth = width;
	_gBufferHeight = height;
	_far = 100.0f;

	// prepare framebuffer and shadow cubemap (texture)
	glGenFramebuffers(1, &_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

	// IMPORTANT: depthMap is a CUBEMAP!
	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _depthMap);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, _gBufferWidth, _gBufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glTexParameteri(OpenGLTexTargets[IMG_CUBE], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(OpenGLTexTargets[IMG_CUBE], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(OpenGLTexTargets[IMG_CUBE], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(OpenGLTexTargets[IMG_CUBE], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(OpenGLTexTargets[IMG_CUBE], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (MOBILE) {
		glTexParameteri(OpenGLTexTargets[IMG_CUBE], GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(OpenGLTexTargets[IMG_CUBE], GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	}

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthMap, 0);
	glDrawBuffers(0, GL_NONE);
	//glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// aspect is 1.0f: Width == Height
	// 90 degree FOV is important to properly align each face of the cubemap
	_projMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, _far);

	_viewProjMatrices.push_back(_projMatrix * 
		glm::lookAt(_position, _position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}

void PointLight::uploadSpatialData(GLuint program) {

	glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, glm::value_ptr(_position));
	glUniform1f(glGetUniformLocation(program, "far"), _far);

	for (int i = 0; i < 6; i++) {
		std::string name = "lightSpaceMatrices[" + std::to_string(i) + "]";
		glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(_viewProjMatrices[i]));
	}
}

void PointLight::uploadShadowMapData(GLuint program) {
	glUniform1f(glGetUniformLocation(program, "far"), _far);
}

void PointLight::updateMatrix() {
	_viewProjMatrices.clear();

	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	_viewProjMatrices.push_back(_projMatrix *
		glm::lookAt(_position, _position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}