/*
	This is where GLUT/GLEW is initialized and the Windows section of the project connects to the core
*/
#include "WindowsApp.h"

#include <iostream>
#include <sstream>

/*
	The current directory for this file is [...] RSM/projects/Windows/RSM
*/

// 1600 x 900
// 800 x 450

#define WND_W 800
#define WND_H 450

using namespace rsm;

void rsm::init(int argc, char* argv[]) {
	glApp = new OpenGLApplication(WND_W, WND_H);

	width = WND_W;
	height = WND_H;
	title = "Mobile Reflective Shadow Maps";

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

	/* ===================================================================================
				Textures
	=====================================================================================*/
	/**/

	/** /
	Image cube_diff;
	cube_diff.loadFromFile("../../../assets/textures/cube/diffuse.png", IMG_2D);
	Image cube_spec;
	cube_spec.loadFromFile("../../../assets/textures/cube/glossiness.png", IMG_2D);
	sref<Texture> cube_diffTex = make_sref<Texture>(cube_diff);
	sref<Texture> cube_specTex = make_sref<Texture>(cube_spec);

	cube_diff.freeImage();
	cube_spec.freeImage();
	/**/


	/**/

	/* ===================================================================================
				Shaders and Materials
	=====================================================================================*/
	/**/

	ShaderSource vShader = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/BlinnPhong.vs");
	ShaderSource fShader = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/BlinnPhong.fs");
	vShader.inject("#version 330 core\n");
	fShader.inject("#version 330 core\n");
	vShader.compile();
	fShader.compile();

	sref<Shader> program = make_sref<Shader>("MainProgram");
	program->addShader(vShader);
	program->addShader(fShader);

	program->link();

	RM.addShader("MainProgram", program);

	glApp->addProgram(program->id());

	/**/
	sref<BlinnPhongMaterial> bp_test = make_sref<BlinnPhongMaterial>();
	bp_test->setDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
	bp_test->setSpecular(glm::vec3(1.0f));
	bp_test->setShininess(32.0f);

	bp_test->setProgram(program->id());
	/**/

	/**/
	/* ===================================================================================
				Meshes and Models
	=====================================================================================*/
	/**/

	/** /
	sref<Model> cube = make_sref<Model>("Cube");
	cube->loadFromFile("../../../assets/models/cube/cube.obj", "../../../assets/models/cube");
	RM.addModel("cube", cube);
	cube->setMaterial(bp_test);
	/**/

	/**/
	sref<Model> sponza = make_sref<Model>("sponza");
	sponza->loadFromFile("../../../assets/models/sponza/sponza.obj", "../../../assets/models/sponza");
	RM.addModel("sponza", sponza);
	/**/

	/** /
	sref<Model> sponza = make_sref<Model>("sponza");
	sponza->loadFromFile("../../../assets/models/crytek sponza/sponza.obj", "../../../assets/models/crytek sponza/");
	RM.addModel("sponza", sponza);
	/**/

	/**/

	checkOpenGLError("Error during loading and setup!");

	glApp->prepare();
}

void rsm::update() {
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	float dt = (float)deltaTime / 1000.0f;
	// Limit the delta time to avoid large intervals
	if (dt > 0.25f)
		dt = 0.25f;

	/** /
	glApp->getCamera()->updateOrientation(mouseDy * dt * 0.75f, mouseDx * dt * 0.75f);
	glApp->getCamera()->updateViewMatrix();

	glm::vec3 moveDir = glm::vec3(0);

	if (keys['w'])
		moveDir += - glApp->getCamera()->front();
	else if (keys['s'])
		moveDir += glApp->getCamera()->front();

	if (keys['d'])
		moveDir += glApp->getCamera()->right();
	else if (keys['a'])
		moveDir += -glApp->getCamera()->right();

	if (moveDir != glm::vec3(0)) {
		glApp->getCamera()->setPosition(glApp->getCamera()->position() + glm::normalize(moveDir) * dt * 10.0f);
		glApp->getCamera()->updateViewMatrix();
	}
	/**/

	glApp->update(dt);
}

void rsm::updateFPS(int value) {
	std::ostringstream oss;
	oss << title << ": " << frameCount << " FPS @ (" << width << "x" << height << ")";
	std::string s = oss.str();
	glutSetWindow(windowHandle);
	glutSetWindowTitle(s.c_str());
	frameCount = 0;
}

void rsm::setTitle(const std::string& new_title) {
	title = new_title;
}

void rsm::render() {
	update();
	glApp->render();
	frameCount++;

	glutSwapBuffers();

	mouseDx = 0;
	mouseDy = 0;
}

void rsm::reshape(int w, int h) {
	glApp->reshape(w, h);
}

void rsm::idle() {
	glutPostRedisplay();
}

void rsm::updateMouse(int x, int y) {
	int dx = -x + mouseX;
	int dy = y - mouseY;

	mouseX = x;
	mouseY = y;

	mouseDx = (mouseDx + (float)dx) / 2.0f;
	mouseDy = (mouseDy + (float)dy) / 2.0f;
}

void rsm::mouseMotion(int x, int y) {
	updateMouse(x, y);
}

void rsm::keyPress(unsigned char key, int x, int y) {
	keys[key] = true;
}

void rsm::keyUp(unsigned char key, int x, int y) {
	keys[key] = false;
}

void rsm::mouseClick(int button, int state, int x, int y) {
	mouseX = x;
	mouseY = y;
}

void rsm::setupCallbacks() {
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutCloseFunc(cleanup);
	glutTimerFunc(1000, updateFPS, 0);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyPress);
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(mouseClick);
}

void rsm::loop() {
	glutMainLoop();
}

void rsm::cleanup() {
	glApp->cleanup();
	delete glApp;
}

int main(int argc, char* argv[]) {
	rsm::init(argc, argv);
	rsm::setupCallbacks();

	rsm::loop();

	exit(EXIT_SUCCESS);
}