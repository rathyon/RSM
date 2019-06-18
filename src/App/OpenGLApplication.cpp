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
		vec3(0.0f, 10.0f, 10.0f),
		vec3(0.0f, 5.0f, 0.0f),
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
	sref<Light> candle = make_sref<PointLight>(glm::vec3(1.0f, 1.0f, 1.0f), 5.0f, glm::vec3(0.0f, 7.0f, 0.0f));
	_scene.addLight(candle);
	candle->prepare(1024, 1024);
	/**/

	/**/
	sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, glm::vec3(-1.0f, -1.0f, -1.0f));
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

	// Prepare shared buffers
	prepareCameraBuffer();

	checkOpenGLError("Error preparing OpenGL Application!");
}

void OpenGLApplication::genRSMaps() {
	GLuint DM = RM.getShader("DepthMap")->id();
	GLuint ODM = RM.getShader("OmniDepthMap")->id();
	GLuint GB = RM.getShader("GBuffer")->id();

	GLuint prog;

	const std::vector<sref<Light>>& lights = _scene.lights();
	for (int l = 0; l < NUM_LIGHTS; l++) {

		glViewport(0, 0, lights[l]->gBufferWidth(), lights[l]->gBufferHeight());

		// if directional light/spotlight
		if (lights[l]->depthMapType() == OpenGLTexTargets[IMG_2D]) {
			uploadLights(GB);

			glUseProgram(GB);
			glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->gBuffer());

			// depth
			//glCullFace(GL_FRONT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			lights[l]->uploadSpatialData(GB);

			glUseProgram(GB);
			_scene.draw(GB);

		}
		// if point light
		else {
			prog = ODM;
			glUseProgram(prog);
	
			lights[l]->uploadSpatialData(prog);

			glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->gBuffer());
			//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (int face = 0; face < 6; face++) {

				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, lights[l]->depthMap(), 0);
				glUniform1i(glGetUniformLocation(prog, "face"), face);

				_scene.draw(prog);
			}

            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lights[l]->depthMap(), 0);
		}

		/** /
		// turn off front face culling for this part
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->_positionFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(GB);
		glUniform1i(glGetUniformLocation(GB, "mode"), 0);
		lights[l]->uploadSpatialData(GB);
		_scene.draw(GB);

		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->_normalFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(GB);
		glUniform1i(glGetUniformLocation(GB, "mode"), 1);
		lights[l]->uploadSpatialData(GB);
		_scene.draw(GB);
		/**/

	}

	glCullFace(GL_BACK);
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// reset viewport
	reshape(_width, _height);
}

void OpenGLApplication::render() { 
	// Generate Depth Map
	genRSMaps();
    checkOpenGLError("Error generating depth maps!");

	// clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// upload lights...
	for (GLuint prog : _programs) {
		uploadLights(prog);
	}
    checkOpenGLError("Error uploading lights data!");

	// upload camera...
	uploadCameraBuffer();
    checkOpenGLError("Error uploading camera data!");

	//upload shadow mapping data
	uploadShadowMappingData();
    checkOpenGLError("Error uploading shadow mapping data!");

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

void OpenGLApplication::uploadShadowMappingData() {
	const std::vector<sref<Light>>& lights = _scene.lights();

	for (GLuint prog : _programs) {
		glUseProgram(prog);
		for (int l = 0; l < NUM_LIGHTS; l++) {
			lights[l]->uploadShadowMapData(prog);

			// TODO: define Tex Unit number for depth maps
			GLenum type = lights[l]->depthMapType();
			if (type == OpenGLTexTargets[IMG_2D]) {

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(type, lights[l]->depthMap());
				glUniform1i(glGetUniformLocation(prog, "depthMap"), 1);

				/**/
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(type, lights[l]->positionMap());
				glUniform1i(glGetUniformLocation(prog, "positionMap"), 3);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(type, lights[l]->normalMap());
				glUniform1i(glGetUniformLocation(prog, "normalMap"), 4);

				glActiveTexture(GL_TEXTURE5);
				glBindTexture(type, lights[l]->fluxMap());
				glUniform1i(glGetUniformLocation(prog, "fluxMap"), 5);
				/**/
			}
			else {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(type, lights[l]->depthMap());
				glUniform1i(glGetUniformLocation(prog, "depthCubeMap"), 2);
			}

		}
	}
}