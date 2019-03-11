#include "OpenGLApplication.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace rsm;

OpenGLApplication::OpenGLApplication() {
}

void OpenGLApplication::init() {
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
	//checkOpenGLError("Error!");
}

int OpenGLApplication::getWidth() {
	return _width;
}
int OpenGLApplication::getHeight() {
	return _height;
}