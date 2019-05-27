#include "OpenGLApplication.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace rsm;

OpenGLApplication::OpenGLApplication(int width, int height) {
	_width = width;
	_height = height;
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
}

Scene OpenGLApplication::getScene() {
	return _scene;
}

sref<Camera> OpenGLApplication::getCamera() {
	return _camera;
}

void OpenGLApplication::prepare() {

	/* ===================================================================================
			Cameras
	=====================================================================================*/

	// cube cam
	/**/
	_camera = make_sref<Perspective>(_width, _height,
		vec3(5.0f, 5.0f, 5.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 1000000.0f, 60.0f);
	/**/

	// sponza cam
	/** /
    _camera = make_sref<Perspective>(_width, _height,
         vec3(0.0f, 15.0f, 0.0f),
         vec3(-15.0f, 15.0f, 0.0f),
         vec3(0.0f, 1.0f, 0.0f),
         0.1f, 1000000.0f, 60.0f);
	/**/

	_scene.addCamera(_camera);

	/* ===================================================================================
			Lights
	=====================================================================================*/

	/** /
	sref<SpotLight> spot = make_sref<SpotLight>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 60.0f, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0, 15.0f, 0));
	_scene.addLight(spot);
	/**/

	/** /
	sref<Light> candle = make_sref<PointLight>(glm::vec3(1.0f, 1.0f, 1.0f), 3.0f, glm::vec3(-3.0f, 4.0f, 3.0f));
	_scene.addLight(candle);
	/**/

	/**/
	sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, glm::vec3(-1.0f, -1.0f, -1.0f));
	_scene.addLight(sun);
	/**/

	/* ===================================================================================
			Models
	=====================================================================================*/

	/** /
	sref<Model> cube = RM.getModel("cube");
	cube->prepare();
	_scene.addModel(cube);

	cube->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
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

	/**/
	sref<Model> demo_scene = RM.getModel("demo_scene");
	demo_scene->prepare();
	_scene.addModel(demo_scene);
	/**/

	// Prepare shared buffers
	prepareCameraBuffer();

	/* ===================================================================================
			Shadow Mapping
	=====================================================================================*/
	// prepare framebuffer and shadow map (texture)
	glGenFramebuffers(1, &_depthMapFBO);

	glGenTextures(1, &_depthMap);
	glBindTexture(OpenGLTexTargets[IMG_2D], _depthMap);
	glTexImage2D(OpenGLTexTargets[IMG_2D], 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(OpenGLTexTargets[IMG_2D], GL_TEXTURE_WRAP_S, OpenGLTexWrapping[REPEAT]);
	glTexParameteri(OpenGLTexTargets[IMG_2D], GL_TEXTURE_WRAP_T, OpenGLTexWrapping[REPEAT]);
	glTexParameteri(OpenGLTexTargets[IMG_2D], GL_TEXTURE_MIN_FILTER, OpenGLTexFilters[NEAREST]);
	glTexParameteri(OpenGLTexTargets[IMG_2D], GL_TEXTURE_MAG_FILTER, OpenGLTexFilters[NEAREST]);

	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// each light source should be in charge of this
	// this means genShadowMap must be called for each light
	// which means each light must have its own framebuffer, texture, etc?
	_lightOrtho = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10000.0f);
	_lightView = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f),
							 glm::vec3(0.0f, 0.0f, 0.0f),
							 glm::vec3(0.0f, 1.0f, 0.0f));
	_lightSpace = _lightOrtho * _lightView;
}

void OpenGLApplication::genShadowMap() {
	sref<Shader> SM = RM.getShader("ShadowMap");
	glUseProgram(SM->id());
	glUniformMatrix4fv(glGetUniformLocation(SM->id(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(_lightSpace));

	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _depthMap);
	_scene.draw(SM);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	// reset viewport
	reshape(_width, _height);
}

void OpenGLApplication::render() { // receive objects and camera args
	// Generate Depth Map
	genShadowMap();

	// clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Upload constant buffers to the GPU

	// upload lights...
	uploadLights();
	// upload camera...
	uploadCameraBuffer();

	//upload shadow mapping data
	uploadShadowMappingData();

	// render objects...
	_scene.render();

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

void OpenGLApplication::uploadLights() {
	const std::vector<sref<Light>>& lights = _scene.lights();
	LightData data;
	std::string name;
	std::string prefix;

	for (GLuint prog : _programs) {
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

			name = prefix + "intensity";
			glUniform1f(glGetUniformLocation(prog, name.c_str()), data.intensity);

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
	
}

void OpenGLApplication::uploadShadowMappingData() {
	for (GLuint prog : _programs) {
		glUseProgram(prog);
		glUniformMatrix4fv(glGetUniformLocation(prog, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(_lightSpace));
	}
}