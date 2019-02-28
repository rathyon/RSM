/*
	This is where GLUT/GLEW is initialized and the Windows section of the project connects to the core
*/
#include "WindowsApp.h"

#include <iostream>
#include <sstream>

#define WND_W 800
#define WND_H 640

using namespace gir;

void gir::init(int argc, char* argv[]) {
	glApp = new OpenGLApplication();

	width = WND_W;
	height = WND_H;
	title = "Global Illumination Rendering";

	// Setup glut
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	windowHandle = glutCreateWindow(title.c_str());

	if (windowHandle < 1) {
		std::cerr << "ERROR: Could not create a rendering window." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Setup glew
	glewExperimental = GL_TRUE;
	GLenum result = glewInit();
	if (result != GLEW_OK) {
		std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
		exit(EXIT_FAILURE);
	}
	GLenum err_code = glGetError();

	// Print system info
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::cerr << "OpenGL Renderer: " << renderer << " (" << vendor << ")" << std::endl;
	std::cerr << "OpenGL version " << version << std::endl;
	std::cerr << "GLSL version " << glslVersion << std::endl;

	/* ===================================================================================
				Loading and Setup
	=====================================================================================*/
	glApp->init();
	RM.init();

	Shader vShader = Shader(VERTEX_SHADER, "../../shaders/vertex.vs");
	vShader.inject("#version 330 core\n");

	Shader fShader = Shader(FRAGMENT_SHADER, "../../shaders/frag.fs");
	fShader.inject("#version 330 core\n");

	sref<Shader> program = make_sref<Shader>("ShaderProgram");

	vShader.compile();
	fShader.compile();

	program->attach(vShader);
	program->attach(fShader);

	RM.addShader("program", program);

	glApp->prepare();
}

void gir::update() {
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	float dt = (float)deltaTime / 1000.0f;
	// Limit the delta time to avoid large intervals
	if (dt > 0.25f)
		dt = 0.25f;

	glApp->update(dt);
}

void gir::updateFPS(int value) {
	std::ostringstream oss;
	oss << title << ": " << frameCount << " FPS @ (" << width << "x" << height << ")";
	std::string s = oss.str();
	glutSetWindow(windowHandle);
	glutSetWindowTitle(s.c_str());
	frameCount = 0;
}

void gir::setTitle(const std::string& new_title) {
	title = new_title;
}

void gir::render() {
	update();
	glApp->render();
	frameCount++;

	glutSwapBuffers();
}

void gir::reshape(int w, int h) {
	glApp->reshape(w, h);
}

void gir::idle() {
	glutPostRedisplay();
}

void gir::setupCallbacks() {
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutCloseFunc(cleanup);
	glutTimerFunc(1000, updateFPS, 0);
}

void gir::loop() {
	glutMainLoop();
}

void gir::cleanup() {
	glApp->cleanup();
	delete glApp;
}

int main(int argc, char* argv[]) {
	gir::init(argc, argv);
	gir::setupCallbacks();

	gir::loop();

	exit(EXIT_SUCCESS);
}