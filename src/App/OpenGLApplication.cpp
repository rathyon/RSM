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
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	//glEnable(GL_MULTISAMPLE);
}

int OpenGLApplication::getWidth() {
	return _width;
}
int OpenGLApplication::getHeight() {
	return _height;
}

void OpenGLApplication::cleanup() {

}

void OpenGLApplication::update(float dt) {
	//_camera->updateViewMatrix();
}

void OpenGLApplication::reshape(int w, int h) {
	_width = w;
	_height = h;
	glViewport(0, 0, w, h);
}


void OpenGLApplication::prepare() {

	/* Prepare Camera here */

	_camera = make_sref<Perspective>(_width, _height,
		vec3(5.0f, 5.0f, 5.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 100.0f, 30.0f);

	_scene.addCamera(_camera);

	/* Prepare Lights here */

	sref<Light> candle = make_sref<PointLight>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, glm::vec3(0.0f, 20.0f, 0.0f));
	_scene.addLight(candle);

	/* Prepare Models here */

	sref<Model> cube = RM.getModel("test_cube");
	cube->prepare();
	cube->setObjToWorld(
		glm::mat4(1.f,0.f,0.f,0.f,
				  0.f,1.f,0.f,0.f,
				  0.f,0.f,1.f,0.f,
				  -0.5f,-0.5f,-0.5f,1.f)
	);

	_scene.addModel(cube);


	// Prepare shared buffers
	prepareCameraBuffer();
	prepareLightsBuffer();

}

void OpenGLApplication::render() { // receive objects and camera args
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Upload constant buffers to the GPU

	// upload lights...
	uploadLightsBuffer();
	// upload camera...
	uploadCameraBuffer();

	// draw objects...
	_scene.draw();

	checkOpenGLError("Error!");
}

void OpenGLApplication::prepareCameraBuffer() {
	glGenBuffers(1, &_cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, _cameraBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), 0, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BUFFER_IDX, _cameraBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void OpenGLApplication::prepareLightsBuffer() {
	glGenBuffers(1, &_lightsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, _lightsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData) * NUM_LIGHTS, 0, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTS_BUFFER_IDX, _lightsBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLApplication::uploadCameraBuffer() {
	CameraData data;
	_camera->toData(data);

	glBindBuffer(GL_UNIFORM_BUFFER, _cameraBuffer);
	GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &data, sizeof(CameraData));
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLApplication::uploadLightsBuffer() {
	const std::vector<sref<Light>>& lights = _scene.lights();

	LightData data[NUM_LIGHTS];
	memset(data, 0, sizeof(LightData) * NUM_LIGHTS);

	//check actual number of lights before copying light data
	// lights.size is size_t, be careful...
	int numLights = std::min(NUM_LIGHTS, (int)lights.size());
	for (int l = 0; l < numLights; l++)
		lights[l]->toData(data[l]);

	glBindBuffer(GL_UNIFORM_BUFFER, _lightsBuffer);
	GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &data, sizeof(LightData) * NUM_LIGHTS);

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}