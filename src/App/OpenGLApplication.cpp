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
	_camera->updateViewMatrix();
}

void OpenGLApplication::reshape(int w, int h) {
	_width = w;
	_height = h;
	_camera->updateProjMatrix(w, h);
	glViewport(0, 0, w, h);
}


void OpenGLApplication::prepare() {

	/* Prepare Camera here */

	_camera = make_sref<Perspective>(_width, _height,
		vec3(2.0f, 2.0f, 2.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 100000.0f, 60.0f);

	_scene.addCamera(_camera);

	/* Prepare Lights here */

	sref<Light> candle = make_sref<PointLight>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	_scene.addLight(candle);

	/* Prepare Models here */

	/** /
	sref<Model> cube = RM.getModel("test_cube");
	cube->prepare();
	_scene.addModel(cube);
	/**/

	/** /
	sref<Model> bunny = RM.getModel("bunny");
	bunny->prepare();
	_scene.addModel(bunny);
	/**/

	/** /
	sref<Model> sponza = RM.getModel("sponza");
	sponza->prepare();
	sponza->setScale(1.0f, 1.0f, 1.0f);
	_scene.addModel(sponza);
	/**/

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
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), &data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLApplication::uploadLightsBuffer() {
	/**/
	const std::vector<sref<Light>>& lights = _scene.lights();

	LightData data[NUM_LIGHTS];
	memset(data, 0, sizeof(LightData) * NUM_LIGHTS);

	//check actual number of lights before copying light data
	int numLights = std::min(NUM_LIGHTS, (int)lights.size());
	for (int l = 0; l < numLights; l++)
		lights[l]->toData(data[l]);

	/** /
	glBindBuffer(GL_UNIFORM_BUFFER, _lightsBuffer);
	GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &data, sizeof(LightData) * NUM_LIGHTS);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	/**/
	glBindBuffer(GL_UNIFORM_BUFFER, _lightsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData)*NUM_LIGHTS, data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	/**/

	/** /
	GLuint prog = RM.getShader("MainProgram")->id();
	GLuint blockIndex = glGetUniformBlockIndex(prog, "lightBlock");
	LOG("BlockIndex for lightBlock is: ");
	LOG(blockIndex);

	GLint blockSize;
	glGetActiveUniformBlockiv(prog, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	LOG("BlockSize for lightBlock is: ");
	LOG(blockSize);

	const GLchar* names[] = {
		"lights[0].position",
		"lights[0].emission",
		"lights[0].intensity",
		"lights[0].type",
		"lights[0].state"
	};

	GLuint indices[5];
	glGetUniformIndices(prog, 5, names, indices);
	LOG("Indices");
	LOG(indices[0]);
	LOG(indices[1]);
	LOG(indices[2]);
	LOG(indices[3]);
	LOG(indices[4]);

	GLint offset[5];
	glGetActiveUniformsiv(prog, 5, indices, GL_UNIFORM_OFFSET, offset);
	LOG("Offsets");
	LOG(offset[0]);
	LOG(offset[1]);
	LOG(offset[2]);
	LOG(offset[3]);
	LOG(offset[4]);

	GLfloat position[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat emission[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat intensity = 1.0f;
	GLint type = 1;
	GLboolean state = true;

	GLubyte* blockBuffer = (GLubyte*)malloc(blockSize);

	memcpy(blockBuffer + offset[0], position, 4 * sizeof(GLfloat));
	memcpy(blockBuffer + offset[1], emission, 4 * sizeof(GLfloat));
	memcpy(blockBuffer + offset[2], &intensity, sizeof(GLfloat));
	memcpy(blockBuffer + offset[3], &type, sizeof(GLint));
	memcpy(blockBuffer + offset[4], &state, sizeof(GLboolean));

	glBindBuffer(GL_UNIFORM_BUFFER, _lightsBuffer);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	std::cin.get();
	/**/
}