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

void OpenGLApplication::prepare() {

	/* Prepare Camera here */
	sref<Camera> mainCamera = make_sref<Perspective>(_width, _height,
		vec3(0.0f, 10.0f, -10.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 500.0f, 60.0f);

	_scene.addCamera(mainCamera);

	/* Prepare Lights here */
	sref<Light> candle = make_sref<PointLight>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, glm::vec3(0.0f, 5.0f, 0.0f));

	_scene.addLight(candle);

	/* Prepare Models here */

	sref<Model> cube = RM.getModel("test_cube");
	cube->prepare();
	cube->setPosition(glm::vec3(0, 0, 0));
	cube->setScale(1.0f, 1.0f, 1.0f);
	cube->updateMatrix();

	_scene.addModel(RM.getModel("test_cube"));

}

void OpenGLApplication::cleanup() {

}

void OpenGLApplication::update(float dt) {
	
}

void OpenGLApplication::reshape(int w, int h) {
	_width = w;
	_height = h;
	glViewport(0, 0, w, h);
}

void OpenGLApplication::render() { // receive objects and camera args
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Upload constant buffers to the GPU

	// upload lights...
	// upload camera...

	// draw objects...

	glUseProgram(0);
	checkOpenGLError("Error!");
}

int OpenGLApplication::getWidth() {
	return _width;
}
int OpenGLApplication::getHeight() {
	return _height;
}