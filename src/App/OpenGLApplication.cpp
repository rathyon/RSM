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
	//glFrontFace(GL_CCW);
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
	// TODO: RESET GBUFFER PARAMS!
}

Scene OpenGLApplication::getScene() {
	return _scene;
}

sref<Camera> OpenGLApplication::getCamera() {
	return _camera;
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

void OpenGLApplication::prepare() {

	/* ===================================================================================
			Cameras
	=====================================================================================*/

	// def cam
	/**/
	_camera = make_sref<Perspective>(_width, _height,
		vec3(-5.0f, 3.0f, -6.0f),
		vec3(0.0f, 3.5f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 1000.0f, 60.0f);
	/**/

	// sponza cam
	/** /
    _camera = make_sref<Perspective>(_width, _height,
         vec3(0.0f, 15.0f, 0.0f),
         vec3(-15.0f, 15.0f, 0.0f),
         vec3(0.0f, 1.0f, 0.0f),
         0.1f, 100000.0f, 60.0f);
	/**/

	// sponza dir light cam
	/** /
	_camera = make_sref<Perspective>(_width, _height,
		vec3(100.0f, 100.0f, 0.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 10000.0f, 60.0f);
	/**/

	_scene.addCamera(_camera);

	/* ===================================================================================
			Lights
	=====================================================================================*/

	/** /
	sref<SpotLight> spot = make_sref<SpotLight>(glm::vec3(1.0f, 1.0f, 1.0f), 3.0f, 30.0f, glm::vec3(-0.5f, -1.0f, -0.5f), glm::vec3(2.0f, 8.0f, 2.0f));
	_scene.addLight(spot);
	spot->prepare(1024);
	/**/

	/** /
	sref<Light> candle = make_sref<PointLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 7.0f, 2.0f));
	_scene.addLight(candle);
	candle->prepare(1024, 1024);
	/**/

	/**/
	sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, -1.0f));
	_scene.addLight(sun);
	sun->prepare(_width, _height);
	/**/

	/* ===================================================================================
			Models
	=====================================================================================*/

	/** /
	sref<Model> cube = RM.getModel("cube");
	cube->prepare();
	_scene.addModel(cube);

	//cube->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	/**/

	/** /
	sref<Model> sponza = RM.getModel("sponza");
	sponza->prepare();
	_scene.addModel(sponza);

	sponza->setScale(0.05f, 0.05f, 0.05f);
	/**/

	/** /
	sref<Model> sibenik = RM.getModel("sibenik");
	sibenik->prepare();
	_scene.addModel(sibenik);
	/**/

	/** /
	sref<Model> demo_scene = RM.getModel("demo_scene");
	demo_scene->prepare();
	_scene.addModel(demo_scene);
	/**/

	/**/
	sref<Model> Lucy = RM.getModel("Lucy");
	Lucy->prepare();
	_scene.addModel(Lucy);
	/**/

	/** /
	sref<Model> demo2 = RM.getModel("demo2");
	demo2->prepare();
	_scene.addModel(demo2);
	//demo2->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	/**/

	// Prepare shared buffers
	prepareCameraBuffer();

	/* ===================================================================================
		DEFERRED SHADING
	=====================================================================================*/
	glGenFramebuffers(1, &_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

	// - position color buffer
	glGenTextures(1, &_gPosition);
	glBindTexture(GL_TEXTURE_2D, _gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &_gNormal);
	glBindTexture(GL_TEXTURE_2D, _gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);

	// diffuse
	glGenTextures(1, &_gDiffuse);
	glBindTexture(GL_TEXTURE_2D, _gDiffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gDiffuse, 0);

	// specular
	glGenTextures(1, &_gSpecular);
	glBindTexture(GL_TEXTURE_2D, _gSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _gBufferWidth, _gBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _gSpecular, 0);

	// - light space position color buffer (need W component)
	glGenTextures(1, &_gLightSpacePosition);
	glBindTexture(GL_TEXTURE_2D, _gLightSpacePosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _gBufferWidth, _gBufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, _gLightSpacePosition, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, _gBufferWidth, _gBufferHeight);
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

	/* ===================================================================================
		RSM
	=====================================================================================*/
	_rsmRMax = VPL_DIST_MAX;
	_rsmIntensity = RSM_INTENSITY;
	// precalculate sampling pattern

	for (int i = 0; i < NUM_VPL; i++) {
		double* sample = hammersley(i, 2, NUM_VPL);
		VPLSamples[i][0] = (float) sample[0];
		VPLSamples[i][1] = (float) sample[1];
	}

	// upload RSM data
	for (GLuint prog : _programs) {
		glUseProgram(prog);
		for (int i = 0; i < NUM_VPL; i++) {
			std::string name = "VPLSamples[" + std::to_string(i) + "]";
			glUniform2fv(glGetUniformLocation(prog, name.c_str()), 1, glm::value_ptr(glm::vec2(VPLSamples[i][0], VPLSamples[i][1])));
		}
		glUniform1f(glGetUniformLocation(prog, "rsmRMax"), _rsmRMax);
		glUniform1f(glGetUniformLocation(prog, "rsmIntensity"), _rsmIntensity);
	}
	glUseProgram(0);

	// prepare low res indirect illumination buffer

	// max dist, dist weight, max "angle", angle weight
	// dist = distance (world space) between frag pos and sample pos
	// "angle" = cos( angle between two normals ) -> think of crease angle (answers question: "is it a hard edge or soft edge?")
	// e.g max dist = 10 world space units; cos(45deg) 
	_indirectSampleParams = glm::vec4(4.0f, 1.0f, glm::cos(glm::radians(45.0f)), 1.0f);

	_indirectLowResWidth = 256;
	_indirectLowResHeight = 256;
	glGenFramebuffers(1, &_indirectLowResFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _indirectLowResFBO);

	glGenTextures(1, &_indirectLowResMap);
	glBindTexture(GL_TEXTURE_2D, _indirectLowResMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _indirectLowResWidth, _indirectLowResHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _indirectLowResMap, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachment[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachment);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOGE("Framebuffer not complete!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	// WARNING: THIS CALL ASSUMES ONLY 1 LIGHT PRESENT!
	_scene.lights()[0]->uploadSpatialData(GB);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_scene.draw(GB);

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	reshape(_width, _height);
}

void OpenGLApplication::genRSMaps() {
	GLuint GB = RM.getShader("RSMGBuffer")->id();
	uploadLights(GB);

	const std::vector<sref<Light>>& lights = _scene.lights();
	for (int l = 0; l < NUM_LIGHTS; l++) {

		glViewport(0, 0, lights[l]->gBufferWidth(), lights[l]->gBufferHeight());

		// if directional light/spotlight
		if (lights[l]->depthMapType() == OpenGLTexTargets[IMG_2D]) {

			glUseProgram(GB);
			glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->gBuffer());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			lights[l]->uploadSpatialData(GB);
			glUniform1i(glGetUniformLocation(GB, "lightIdx"), l);

			_scene.draw(GB);
		}

		/** /
		// if point light
		glUseProgram(OGB);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->gBuffer());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lights[l]->uploadSpatialData(OGB);
		for (int face = 0; face < 6; face++) {
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, lights[l]->depthMap(), 0);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, lights[l]->positionMap(), 0);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, lights[l]->normalMap(), 0);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, lights[l]->fluxMap(), 0);
			glUniform1i(glGetUniformLocation(OGB, "face"), face);
			glUniform1i(glGetUniformLocation(OGB, "lightIdx"), l);
			_scene.draw(OGB);
		}
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lights[l]->depthMap(), 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, lights[l]->positionMap(), 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, lights[l]->normalMap(), 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, lights[l]->fluxMap(), 0);
		/**/
	}

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// reset viewport
	reshape(_width, _height);
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

	// Geometry Pass (Deferred Shading) GBuffer...
	geometryPass();
	checkOpenGLError("Error in geometry pass!");

	// Clear framebuffer...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Upload deferred shading data...
	uploadDeferredShadingData();
	checkOpenGLError("Error uploading deferred shading data!");

	// Upload shadow mapping data...
	uploadShadowMappingData();
    checkOpenGLError("Error uploading shadow mapping data!");

	// render low res indirect illumination
	renderLowResIndirect();
	uploadLowResIndirect();

	// render high res indirect illumination?

	// render direct illumination + pre calc indirect
	// Render scene...
	//_scene.render();
	renderScreenQuad();

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
	const std::vector<sref<Light>>& lights = _scene.lights();
	LightData data;
	std::string name;
	std::string prefix;

	glUseProgram(prog);

	// I'll be assuming NUM_LIGHTS is always the actual number of lights in the scene at all times
	for (int l = 0; l < NUM_LIGHTS; l++) {
		lights[l]->toData(data);

		prefix = "lights[" + std::to_string(l) + "].";

		name = prefix + "position";
		glUniform3fv(glGetUniformLocation(prog, name.c_str()), 1, glm::value_ptr(data.position));

		name = prefix + "direction";
		glUniform3fv(glGetUniformLocation(prog, name.c_str()), 1, glm::value_ptr(data.direction));

		name = prefix + "emission";
		glUniform3fv(glGetUniformLocation(prog, name.c_str()), 1, glm::value_ptr(data.emission));

		name = prefix + "linear";
		glUniform1f(glGetUniformLocation(prog, name.c_str()), data.linear);

		name = prefix + "quadratic";
		glUniform1f(glGetUniformLocation(prog, name.c_str()), data.quadratic);

		name = prefix + "type";
		glUniform1i(glGetUniformLocation(prog, name.c_str()), data.type);

		name = prefix + "state";
		glUniform1i(glGetUniformLocation(prog, name.c_str()), data.state);

		name = prefix + "cutoff";
		glUniform1f(glGetUniformLocation(prog, name.c_str()), data.cutoff);
	}
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

		glActiveTexture(OpenGLTextureUnits[TextureUnit::G_SPECULAR]);
		glBindTexture(GL_TEXTURE_2D, _gSpecular);
		glUniform1i(glGetUniformLocation(prog, "gSpecular"), TextureUnit::G_SPECULAR);

		glActiveTexture(OpenGLTextureUnits[TextureUnit::G_LIGHTSPACEPOSITION]);
		glBindTexture(GL_TEXTURE_2D, _gLightSpacePosition);
		glUniform1i(glGetUniformLocation(prog, "gLightSpacePosition"), TextureUnit::G_LIGHTSPACEPOSITION);
	}
}

void OpenGLApplication::uploadShadowMappingData() {
	const std::vector<sref<Light>>& lights = _scene.lights();

	for (GLuint prog : _programs) {
		glUseProgram(prog);
		for (int l = 0; l < NUM_LIGHTS; l++) {
			lights[l]->uploadShadowMapData(prog);

			GLenum type = lights[l]->depthMapType();

				glActiveTexture(OpenGLTextureUnits[TextureUnit::RSM_DEPTH]);
				glBindTexture(type, lights[l]->depthMap());
				glUniform1i(glGetUniformLocation(prog, "depthMap"), TextureUnit::RSM_DEPTH);

				glActiveTexture(OpenGLTextureUnits[TextureUnit::RSM_POSITION]);
				glBindTexture(type, lights[l]->positionMap());
				glUniform1i(glGetUniformLocation(prog, "positionMap"), TextureUnit::RSM_POSITION);

				glActiveTexture(OpenGLTextureUnits[TextureUnit::RSM_NORMAL]);
				glBindTexture(type, lights[l]->normalMap());
				glUniform1i(glGetUniformLocation(prog, "normalMap"), TextureUnit::RSM_NORMAL);

				glActiveTexture(OpenGLTextureUnits[TextureUnit::RSM_FLUX]);
				glBindTexture(type, lights[l]->fluxMap());
				glUniform1i(glGetUniformLocation(prog, "fluxMap"), TextureUnit::RSM_FLUX);

			/** /
			else {
				glActiveTexture(OpenGLTextureUnits[TextureUnit::OMNI_G_DEPTH]);
				glBindTexture(type, lights[l]->depthMap());
				glUniform1i(glGetUniformLocation(prog, "depthCubeMap"), TextureUnit::OMNI_G_DEPTH);

				glActiveTexture(OpenGLTextureUnits[TextureUnit::OMNI_G_POSITION]);
				glBindTexture(type, lights[l]->positionMap());
				glUniform1i(glGetUniformLocation(prog, "positionCubeMap"), TextureUnit::OMNI_G_POSITION);

				glActiveTexture(OpenGLTextureUnits[TextureUnit::OMNI_G_NORMAL]);
				glBindTexture(type, lights[l]->normalMap());
				glUniform1i(glGetUniformLocation(prog, "normalCubeMap"), TextureUnit::OMNI_G_NORMAL);

				glActiveTexture(OpenGLTextureUnits[TextureUnit::OMNI_G_FLUX]);
				glBindTexture(type, lights[l]->fluxMap());
				glUniform1i(glGetUniformLocation(prog, "fluxCubeMap"), TextureUnit::OMNI_G_FLUX);
			}
			/**/

		}
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