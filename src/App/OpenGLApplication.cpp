#include "OpenGLApplication.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace rsm;

OpenGLApplication::OpenGLApplication(int width, int height) {
	_width = width;
	_height = height;

	_gBufferWidth = width;
	_gBufferHeight = height;
}

void OpenGLApplication::init() {
	// init Scene
	_scene = Scene();

	// init random seed
	srand(static_cast <unsigned> (time(0)));

	// Initialize OpenGL state
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int OpenGLApplication::getWidth() {
	return _width;
}
int OpenGLApplication::getHeight() {
	return _height;
}

void OpenGLApplication::addProgram(GLuint prog) {
	_programs.push_back(prog);
}

void OpenGLApplication::cleanup() {

}

void OpenGLApplication::update(float dt) {

}

void OpenGLApplication::reshape(int w, int h) {
	_width = w;
	_height = h;
	_camera->updateProjMatrix(w, h);
	glViewport(0, 0, w, h);
	// TODO: RESET GBUFFER PARAMS! -> is it needed?
}

Scene* OpenGLApplication::getScene() {
	return &_scene;
}

sref<Camera> OpenGLApplication::getCamera() {
	return _camera;
}

void OpenGLApplication::setCamera(const sref<Camera>& camera) {
	_camera = camera;
}

std::vector<GLuint> OpenGLApplication::programs() {
	return _programs;
}

float OpenGLApplication::rsmRMax() {
	return _rsmRMax;
}

float OpenGLApplication::rsmIntensity() {
	return _rsmIntensity;
}

void OpenGLApplication::setRSMRMax(float val) {
	_rsmRMax = val;
}

void OpenGLApplication::setRSMIntensity(float val) {
	_rsmIntensity = val;
}

void OpenGLApplication::prepareDeferredShading() {
	glGenFramebuffers(1, &_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

	// - position color buffer
	glGenTextures(1, &_gPosition);
	glBindTexture(GL_TEXTURE_2D, _gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &_gNormal);
	glBindTexture(GL_TEXTURE_2D, _gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);

	// diffuse
	glGenTextures(1, &_gDiffuse);
	glBindTexture(GL_TEXTURE_2D, _gDiffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gDiffuse, 0);

	// - light space position color buffer (need W component)
	glGenTextures(1, &_gLightSpacePosition);
	glBindTexture(GL_TEXTURE_2D, _gLightSpacePosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _gBufferWidth, _gBufferHeight, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _gLightSpacePosition, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _gBufferWidth, _gBufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOGE("Framebuffer not complete!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Screen Quad Setup

	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	// setup plane VAO
	glGenVertexArrays(1, &_screenQuadVAO);
	glGenBuffers(1, &_screenQuadVBO);

	glBindVertexArray(_screenQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _screenQuadVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(AttributeType::POSITION);
	glVertexAttribPointer(AttributeType::POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(AttributeType::UV);
	glVertexAttribPointer(AttributeType::UV, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void OpenGLApplication::prepareRSM() {
	_rsmRMax = VPL_DIST_MAX;
	_rsmIntensity = RSM_INTENSITY;
	// precalculate sampling pattern

	for (int i = 0; i < NUM_VPL; i++) {
		double* sample = hammersley(i, 2, NUM_VPL);
		VPLSamples[i][0] = (float)sample[0];
		VPLSamples[i][1] = (float)sample[1];
		VPLWeights[i] = VPLSamples[i][0] * VPLSamples[i][0];
	}

	// upload RSM data
	for (GLuint prog : _programs) {
		glUseProgram(prog);
		for (int i = 0; i < NUM_VPL; i++) {
			std::string name = "VPLSamples[" + std::to_string(i) + "]";
			glUniform2fv(glGetUniformLocation(prog, name.c_str()), 1, glm::value_ptr(glm::vec2(VPLSamples[i][0], VPLSamples[i][1])));
			name = "VPLWeights[" + std::to_string(i) + "]";
			glUniform1f(glGetUniformLocation(prog, name.c_str()), VPLWeights[i]);
		}
		glUniform1f(glGetUniformLocation(prog, "rsmRMax"), _rsmRMax);
		glUniform1f(glGetUniformLocation(prog, "rsmIntensity"), _rsmIntensity);

		glUniform2f(glGetUniformLocation(prog, "lowResIndirectSize"), (float)LOW_RES_INDIRECT_WIDTH, (float)LOW_RES_INDIRECT_HEIGHT);
		glUniform2f(glGetUniformLocation(prog, "texelSize"), 1.0f / (float)LOW_RES_INDIRECT_WIDTH, 1.0f / (float)LOW_RES_INDIRECT_HEIGHT);
	}
	glUseProgram(0);

	// prepare low res indirect illumination buffer

	// max dist, dist weight, max "angle", angle weight
	// dist = SQUARED distance (world space) between frag pos and sample pos
	// "angle" = cos( angle between two normals ) -> think of crease angle (answers question: "is it a hard edge or soft edge?")
	// e.g max dist = 10 world space units; cos(45deg) 
	_indirectSampleParams = glm::vec4(4.0f, 1.0f, glm::cos(glm::radians(45.0f)), 1.0f);

	_indirectLowResWidth = LOW_RES_INDIRECT_WIDTH;
	_indirectLowResHeight = LOW_RES_INDIRECT_HEIGHT;
	glGenFramebuffers(1, &_indirectLowResFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _indirectLowResFBO);

	glGenTextures(1, &_indirectLowResMap);
	glBindTexture(GL_TEXTURE_2D, _indirectLowResMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, _indirectLowResWidth, _indirectLowResHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _indirectLowResMap, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachment[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachment);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOGE("Framebuffer not complete!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLApplication::prepare() {

	// Prepare shared buffers
	prepareCameraBuffer();

#ifdef RSM_DEFERRED
	prepareDeferredShading();
#endif

	prepareRSM();

	checkOpenGLError("Error preparing OpenGL Application!");
}

void OpenGLApplication::renderScreenQuad() {
	GLuint prog = RM.getShader("DeferredShading")->id();

	glUseProgram(prog);
	glBindVertexArray(_screenQuadVAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glBindVertexArray(0);
	glUseProgram(0);
}

void OpenGLApplication::geometryPass() {
	GLuint GB = RM.getShader("GBuffer")->id();

	glViewport(0, 0, _gBufferWidth, _gBufferHeight);
	glUseProgram(GB);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

	_scene.lights()[0]->uploadSpatialData(GB);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_scene.draw(GB);

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0 ,_width, _height);
}

void OpenGLApplication::genRSMaps() {
	glDisable(GL_CULL_FACE);
	GLuint GB = RM.getShader("RSMGBuffer")->id();
	uploadLights(GB);

	const sref<Light>& light = _scene.lights()[0];

	glViewport(0, 0, light->gBufferWidth(), light->gBufferHeight());

	glUseProgram(GB);
	glBindFramebuffer(GL_FRAMEBUFFER, light->gBuffer());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	light->uploadSpatialData(GB);

	_scene.draw(GB);

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// reset viewport
	glViewport(0, 0, _width, _height);
	glEnable(GL_CULL_FACE);
}

void OpenGLApplication::renderLowResIndirect() {
	GLuint prog = RM.getShader("IndirectIllumination")->id();

	glViewport(0, 0, _indirectLowResWidth, _indirectLowResHeight);

	glUseProgram(prog);
	glBindFramebuffer(GL_FRAMEBUFFER, _indirectLowResFBO);
	glBindVertexArray(_screenQuadVAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glViewport(0, 0, _width, _height);
}

void OpenGLApplication::render() { 

	// Upload lights...
	for (GLuint prog : _programs) {
		uploadLights(prog);
	}
	checkOpenGLError("Error uploading lights data!");

	// Upload camera...
	uploadCameraBuffer();
	checkOpenGLError("Error uploading camera data!");

	// Generate RSM GBuffer...
	genRSMaps();
	checkOpenGLError("Error generating depth maps!");

#ifdef RSM_NAIVE
	// Clear framebuffer...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Upload shadow mapping data...
	uploadShadowMappingData();
	checkOpenGLError("Error uploading shadow mapping data!");

	// Render scene...
	_scene.render();
#endif

#ifdef RSM_DEFERRED
	// Geometry Pass (Deferred Shading) GBuffer...
	geometryPass();
	checkOpenGLError("Error in geometry pass!");

	// Clear framebuffer...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Upload deferred shading data...
	uploadDeferredShadingData();
	checkOpenGLError("Error uploading deferred shading data!");

	// Upload deferred shading data...
	uploadDeferredShadingData();
	checkOpenGLError("Error uploading deferred shading data!");

	// Upload shadow mapping data...
	uploadShadowMappingData();
	checkOpenGLError("Error uploading shadow mapping data!");

	renderLowResIndirect();

	uploadLowResIndirect();

	renderScreenQuad();
#endif

	checkOpenGLError("Error in render loop!");
}

void OpenGLApplication::prepareCameraBuffer() {
	glGenBuffers(1, &_cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, _cameraBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), 0, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BUFFER_IDX, _cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void OpenGLApplication::uploadCameraBuffer() {
	CameraData data;
	_camera->toData(data);

	glBindBuffer(GL_UNIFORM_BUFFER, _cameraBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), &data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLApplication::uploadLights(GLuint prog) {
	const sref<Light>& light = _scene.lights()[0];
	LightData data;
	std::string name;

	glUseProgram(prog);

	// I'll be assuming NUM_LIGHTS is always the actual number of lights in the scene at all times
	light->toData(data);

	name = "light.position";
	glUniform3fv(glGetUniformLocation(prog, name.c_str()), 1, glm::value_ptr(data.position));

	name = "light.direction";
	glUniform3fv(glGetUniformLocation(prog, name.c_str()), 1, glm::value_ptr(data.direction));

	name = "light.emission";
	glUniform3fv(glGetUniformLocation(prog, name.c_str()), 1, glm::value_ptr(data.emission));

	name = "light.linear";
	glUniform1f(glGetUniformLocation(prog, name.c_str()), data.linear);

	name = "light.quadratic";
	glUniform1f(glGetUniformLocation(prog, name.c_str()), data.quadratic);

	name = "light.type";
	glUniform1i(glGetUniformLocation(prog, name.c_str()), data.type);

	name = "light.state";
	glUniform1i(glGetUniformLocation(prog, name.c_str()), data.state);

	name = "light.cutoff";
	glUniform1f(glGetUniformLocation(prog, name.c_str()), data.cutoff);
	glUseProgram(0);
}

void OpenGLApplication::uploadDeferredShadingData() {
	for (GLuint prog : _programs) {
		glUseProgram(prog);

		glActiveTexture(OpenGLTextureUnits[TextureUnit::G_POSITION]);
		glBindTexture(GL_TEXTURE_2D, _gPosition);
		glUniform1i(glGetUniformLocation(prog, "gPosition"), TextureUnit::G_POSITION);

		glActiveTexture(OpenGLTextureUnits[TextureUnit::G_NORMAL]);
		glBindTexture(GL_TEXTURE_2D, _gNormal);
		glUniform1i(glGetUniformLocation(prog, "gNormal"), TextureUnit::G_NORMAL);

		glActiveTexture(OpenGLTextureUnits[TextureUnit::G_DIFFUSE]);
		glBindTexture(GL_TEXTURE_2D, _gDiffuse);
		glUniform1i(glGetUniformLocation(prog, "gDiffuse"), TextureUnit::G_DIFFUSE);

		glActiveTexture(OpenGLTextureUnits[TextureUnit::G_LIGHTSPACEPOSITION]);
		glBindTexture(GL_TEXTURE_2D, _gLightSpacePosition);
		glUniform1i(glGetUniformLocation(prog, "gLightSpacePosition"), TextureUnit::G_LIGHTSPACEPOSITION);
	}
}

void OpenGLApplication::uploadShadowMappingData() {
	const sref<Light>& light = _scene.lights()[0];

	for (GLuint prog : _programs) {
		glUseProgram(prog);
		light->uploadShadowMapData(prog);

		glActiveTexture(OpenGLTextureUnits[TextureUnit::RSM_DEPTH]);
		glBindTexture(OpenGLTexTargets[IMG_2D], light->depthMap());
		glUniform1i(glGetUniformLocation(prog, "depthMap"), TextureUnit::RSM_DEPTH);

		glActiveTexture(OpenGLTextureUnits[TextureUnit::RSM_POSITION]);
		glBindTexture(OpenGLTexTargets[IMG_2D], light->positionMap());
		glUniform1i(glGetUniformLocation(prog, "positionMap"), TextureUnit::RSM_POSITION);

		glActiveTexture(OpenGLTextureUnits[TextureUnit::RSM_NORMAL]);
		glBindTexture(OpenGLTexTargets[IMG_2D], light->normalMap());
		glUniform1i(glGetUniformLocation(prog, "normalMap"), TextureUnit::RSM_NORMAL);

		glActiveTexture(OpenGLTextureUnits[TextureUnit::RSM_FLUX]);
		glBindTexture(OpenGLTexTargets[IMG_2D], light->fluxMap());
		glUniform1i(glGetUniformLocation(prog, "fluxMap"), TextureUnit::RSM_FLUX);

	}
}

void OpenGLApplication::uploadLowResIndirect() {
	for (GLuint prog : _programs) {
		glUseProgram(prog);
		glActiveTexture(OpenGLTextureUnits[TextureUnit::LOW_RES_INDIRECT]);
		glBindTexture(GL_TEXTURE_2D, _indirectLowResMap);
		glUniform1i(glGetUniformLocation(prog, "lowResIndirect"), TextureUnit::LOW_RES_INDIRECT);
		glUniform4fv(glGetUniformLocation(prog, "indirectSampleParams"), 1, glm::value_ptr(_indirectSampleParams));
	}

	glUseProgram(0);
}