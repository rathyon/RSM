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
				Shaders
	=====================================================================================*/
	/**/

	ShaderSource vBP = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/BlinnPhong.vs");
	ShaderSource fBP = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/BlinnPhong.fs");
	vBP.inject(std::string("#version 330 core\n"));
	fBP.inject(std::string("#version 330 core\n") +
			   std::string("const int NUM_LIGHTS = ") + std::to_string(NUM_LIGHTS) + ";\n");
	vBP.compile();
	fBP.compile();

	ShaderSource vBPTex = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/BlinnPhongTex.vs");
	ShaderSource fBPTex = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/BlinnPhongTex.fs");
	vBPTex.inject(std::string("#version 330 core\n"));
	fBPTex.inject(std::string("#version 330 core\n") +
		          std::string("const int NUM_LIGHTS = ") + std::to_string(NUM_LIGHTS) + ";\n");
	vBPTex.compile();
	fBPTex.compile();

	ShaderSource vDM = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/DepthMap.vs");
	ShaderSource fDM = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/DepthMap.fs");
	vDM.inject(std::string("#version 330 core\n"));
	fDM.inject(std::string("#version 330 core\n"));
	vDM.compile();
	fDM.compile();

	ShaderSource vGB = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/GBuffer.vs");
	ShaderSource fGB = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/GBuffer.fs");
	vGB.inject(std::string("#version 330 core\n"));
	fGB.inject(std::string("#version 330 core\n"));
	vGB.compile();
	fGB.compile();

	ShaderSource vODM = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/OmniDepthMap.vs");
	//ShaderSource gODM = ShaderSource(GEOMETRY_SHADER, "../../../src/Shaders/OmniDepthMap.gs");
	ShaderSource fODM = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/OmniDepthMap.fs");
	vODM.inject(std::string("#version 330 core\n"));
	//gODM.inject(std::string("#version 330 core\n"));
	fODM.inject(std::string("#version 330 core\n"));
	vODM.compile();
	//gODM.compile();
	fODM.compile();

	sref<Shader> BlinnPhong = make_sref<Shader>("BlinnPhong");
	BlinnPhong->addShader(vBP);
	BlinnPhong->addShader(fBP);
	BlinnPhong->link();
	RM.addShader("BlinnPhong", BlinnPhong);
	glApp->addProgram(BlinnPhong->id());

	sref<Shader> BlinnPhongTex = make_sref<Shader>("BlinnPhongTex");
	BlinnPhongTex->addShader(vBPTex);
	BlinnPhongTex->addShader(fBPTex);
	BlinnPhongTex->link();
	RM.addShader("BlinnPhongTex", BlinnPhongTex);
	glApp->addProgram(BlinnPhongTex->id());

	sref<Shader> DepthMap = make_sref<Shader>("DepthMap");
	DepthMap->addShader(vDM);
	DepthMap->addShader(fDM);
	DepthMap->link();
	RM.addShader("DepthMap", DepthMap);

	sref<Shader> GBuffer = make_sref<Shader>("GBuffer");
	GBuffer->addShader(vGB);
	GBuffer->addShader(fGB);
	GBuffer->link();
	RM.addShader("GBuffer", GBuffer);

	sref<Shader> OmniDepthMap = make_sref<Shader>("OmniDepthMap");
	OmniDepthMap->addShader(vODM);
	//OmniDepthMap->addShader(gODM);
	OmniDepthMap->addShader(fODM);
	OmniDepthMap->link();
	RM.addShader("OmniDepthMap", OmniDepthMap);

	/* ===================================================================================
				Materials
	=====================================================================================*/

	/**/
	sref<BlinnPhongMaterial> bp_red = make_sref<BlinnPhongMaterial>();
	bp_red->setDiffuse(glm::vec3(1.0f, 0.0f, 0.0f));
	bp_red->setSpecular(glm::vec3(1.0f));
	bp_red->setShininess(32.0f);
	bp_red->setProgram(BlinnPhong->id());

	sref<BlinnPhongMaterial> bp_gray = make_sref<BlinnPhongMaterial>();
	bp_gray->setDiffuse(glm::vec3(0.7f, 0.7f, 0.7f));
	bp_gray->setSpecular(glm::vec3(1.0f));
	bp_gray->setShininess(4.0f);
	bp_gray->setProgram(BlinnPhong->id());

	sref<BlinnPhongMaterial> bp_green = make_sref<BlinnPhongMaterial>();
	bp_green->setDiffuse(glm::vec3(0.0f, 1.0f, 0.0f));
	bp_green->setSpecular(glm::vec3(1.0f));
	bp_green->setShininess(64.0f);
	bp_green->setProgram(BlinnPhong->id());
	/**/

	/* ===================================================================================
				Models
	=====================================================================================*/
	/**/

	/** /
	sref<Model> cube = make_sref<Model>("Cube");
	cube->loadFromFile("../../../assets/models/cube/cube.obj", "../../../assets/models/cube");
	RM.addModel("cube", cube);
	cube->setMaterial(bp_red);
	/**/

	/** /
	sref<Model> sponza = make_sref<Model>("sponza");
	sponza->loadFromFile("../../../assets/models/crytek sponza/sponza.obj", "../../../assets/models/crytek sponza/");
	RM.addModel("sponza", sponza);
	sponza->setMaterial(bp_green);
	/**/

	/** /
	sref<Model> sibenik = make_sref<Model>("sibenik");
	sibenik->loadFromFile("../../../assets/models/sibenik/sibenik.obj", "../../../assets/models/sibenik/");
	RM.addModel("sibenik", sibenik);
	/**/

	/** /
	sref<Model> demo_scene = make_sref<Model>("demo_scene");
	demo_scene->loadFromFile("../../../assets/models/demo scene open/demo_scene.obj", "../../../assets/models/demo scene open/");
	RM.addModel("demo_scene", demo_scene);
	/**/

	/** /
	sref<Model> demo_scene = make_sref<Model>("demo_scene");
	demo_scene->loadFromFile("../../../assets/models/demo scene closed/demo_scene.obj", "../../../assets/models/demo scene closed/");
	RM.addModel("demo_scene", demo_scene);
	/**/

	/**/
	sref<Model> Lucy = make_sref<Model>("Lucy");
	Lucy->loadFromFile("../../../assets/models/Lucy/Lucy.obj", "../../../assets/models/Lucy/");
	RM.addModel("Lucy", Lucy);
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

	glApp->getCamera()->updateOrientation(mouseDy * dt * 0.75f, mouseDx * dt * 0.75f);
	glApp->getCamera()->updateViewMatrix();

	glm::vec3 moveDir = glm::vec3(0);

	if (keys['w'])
		moveDir += -glApp->getCamera()->front();
	else if (keys['s'])
		moveDir += glApp->getCamera()->front();

	if (keys['d'])
		moveDir += glApp->getCamera()->right();
	else if (keys['a'])
		moveDir += -glApp->getCamera()->right();

	if (moveDir != glm::vec3(0.0f)) {
		glApp->getCamera()->setPosition(glApp->getCamera()->position() + glm::normalize(moveDir) * dt * 10.0f);
		glApp->getCamera()->updateViewMatrix();
	}

	/**/

	// debug for point light
	/** /
	sref<Light> light = glApp->getScene().lights()[0];
	if (keys['i'])
		light->setPosition(light->position() + glm::vec3(0.0f, 0.0f, 0.05f));
	else if (keys['k'])
		light->setPosition(light->position() + glm::vec3(0.0f, 0.0f, -0.05f));

	if (keys['j'])
		light->setPosition(light->position() + glm::vec3(-0.05f, 0.0f, 0.0f));
	else if (keys['l'])
		light->setPosition(light->position() + glm::vec3(0.05f, 0.0f, 0.0f));

	light->updateMatrix();
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

	if (keys['f'])
		glApp->getCamera()->info();
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