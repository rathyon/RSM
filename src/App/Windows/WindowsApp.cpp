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

	glApp->prepareLights();
	const sref<Light>& light = glApp->getScene().lights()[0];
	LightData data;
	light->toData(data);
	const int lightType = data.type;

	/* ===================================================================================
				Shaders
	=====================================================================================*/
	/**/

	std::string lightDef;
	if (lightType == LightType::LIGHTYPE_DIR)
		lightDef = "#define DIRECTIONAL\n";
	else
		lightDef = "#define SPOTLIGHT\n";

	ShaderSource vBP = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/BlinnPhong.vs");
	ShaderSource fBP = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/BlinnPhong.fs");
	vBP.inject(std::string("#version 330 core\n"));
	fBP.inject(std::string("#version 330 core\n") +
		lightDef +
		std::string("const int NUM_VPL = ") + std::to_string(NUM_VPL) + ";\n");
	vBP.compile();
	fBP.compile();

	ShaderSource vBPT = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/BlinnPhongTex.vs");
	ShaderSource fBPT = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/BlinnPhongTex.fs");
	vBPT.inject(std::string("#version 330 core\n"));
	fBPT.inject(std::string("#version 330 core\n") +
		lightDef +
		std::string("const int NUM_VPL = ") + std::to_string(NUM_VPL) + ";\n");
	vBPT.compile();
	fBPT.compile();

	ShaderSource vDS = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/DeferredShading.vs");
	ShaderSource fDS = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/DeferredShading.fs");
	vDS.inject(std::string("#version 330 core\n"));
	fDS.inject(std::string("#version 330 core\n") +
		lightDef +
		std::string("const int NUM_VPL = ") + std::to_string(NUM_VPL) + ";\n");
	vDS.compile();
	fDS.compile();

	ShaderSource vII = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/IndirectIllumination.vs");
	ShaderSource fII = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/IndirectIllumination.fs");
	vII.inject(std::string("#version 330 core\n"));
	fII.inject(std::string("#version 330 core\n") +
		std::string("const int NUM_VPL = ") + std::to_string(NUM_VPL) + ";\n");
	vII.compile();
	fII.compile();

	ShaderSource vGB = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/GBuffer.vs");
	ShaderSource fGB = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/GBuffer.fs");
	vGB.inject(std::string("#version 330 core\n"));
	fGB.inject(std::string("#version 330 core\n"));
	vGB.compile();
	fGB.compile();

	ShaderSource vRGB = ShaderSource(VERTEX_SHADER, "../../../src/Shaders/RSMGBuffer.vs");
	ShaderSource fRGB = ShaderSource(FRAGMENT_SHADER, "../../../src/Shaders/RSMGBuffer.fs");
	vRGB.inject(std::string("#version 330 core\n"));
	fRGB.inject(std::string("#version 330 core\n"));
	vRGB.compile();
	fRGB.compile();

	sref<Shader> BlinnPhong = make_sref<Shader>("BlinnPhong");
	BlinnPhong->addShader(vBP);
	BlinnPhong->addShader(fBP);
	BlinnPhong->link();
	RM.addShader("BlinnPhong", BlinnPhong);
	glApp->addProgram(BlinnPhong->id());

	sref<Shader> BlinnPhongTex = make_sref<Shader>("BlinnPhongTex");
	BlinnPhongTex->addShader(vBPT);
	BlinnPhongTex->addShader(fBPT);
	BlinnPhongTex->link();
	RM.addShader("BlinnPhongTex", BlinnPhongTex);
	glApp->addProgram(BlinnPhongTex->id());

	sref<Shader> DeferredShading = make_sref<Shader>("DeferredShading");
	DeferredShading->addShader(vDS);
	DeferredShading->addShader(fDS);
	DeferredShading->link();
	RM.addShader("DeferredShading", DeferredShading);
	glApp->addProgram(DeferredShading->id());

	sref<Shader> IndirectIllumination = make_sref<Shader>("IndirectIllumination");
	IndirectIllumination->addShader(vII);
	IndirectIllumination->addShader(fII);
	IndirectIllumination->link();
	RM.addShader("IndirectIllumination", IndirectIllumination);
	glApp->addProgram(IndirectIllumination->id());

	sref<Shader> GBuffer = make_sref<Shader>("GBuffer");
	GBuffer->addShader(vGB);
	GBuffer->addShader(fGB);
	GBuffer->link();
	RM.addShader("GBuffer", GBuffer);

	sref<Shader> RSMGBuffer = make_sref<Shader>("RSMGBuffer");
	RSMGBuffer->addShader(vRGB);
	RSMGBuffer->addShader(fRGB);
	RSMGBuffer->link();
	RM.addShader("RSMGBuffer", RSMGBuffer);

	/* ===================================================================================
				Materials
	=====================================================================================*/

	/** /
	sref<BlinnPhongMaterial> bp_red = make_sref<BlinnPhongMaterial>();
	bp_red->setDiffuse(glm::vec3(1.0f, 0.0f, 0.0f));
	bp_red->setSpecular(glm::vec3(1.0f));
	bp_red->setShininess(32.0f);
	bp_red->setProgram(BlinnPhong->id());
	/**/

	/* ===================================================================================
				Models
	=====================================================================================*/
	/**/


	/**/
	sref<Model> sponza = make_sref<Model>("sponza");
	sponza->loadFromFile("../../../assets/models/crytek sponza/sponza.obj", "../../../assets/models/crytek sponza/");
	RM.addModel("sponza", sponza);
	/**/

	/** /
	sref<Model> demo_scene = make_sref<Model>("demo_scene");
	demo_scene->loadFromFile("../../../assets/models/demo scene open/demo_scene.obj", "../../../assets/models/demo scene open/");
	RM.addModel("demo_scene", demo_scene);
	/**/

	/** /
	sref<Model> Lucy = make_sref<Model>("Lucy");
	Lucy->loadFromFile("../../../assets/models/Lucy/Lucy.obj", "../../../assets/models/Lucy/");
	RM.addModel("Lucy", Lucy);	
	/**/

	/** /
	sref<Model> Lucy = make_sref<Model>("Lucy");
	Lucy->loadFromFile("../../../assets/models/Lucy2/Lucy2.obj", "../../../assets/models/Lucy2/");
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

	if (keys['c'])
		LOG("Camera Pos: %f %f %f\n", glApp->getCamera()->position().x, glApp->getCamera()->position().y, glApp->getCamera()->position().z);

	if (keys['o']) {
		if (glApp->rsmRMax() < 0.999f) {
			glApp->setRSMRMax(glApp->rsmRMax() + 0.001f);
			LOG("rsmRMax = %f\n", glApp->rsmRMax());
			std::vector<GLuint> progs = glApp->programs();

			for (GLuint prog : progs) {
				glUseProgram(prog);
				glUniform1f(glGetUniformLocation(prog, "rsmRMax"), glApp->rsmRMax());
			}
			glUseProgram(0);
		}
	}

	if (keys['i']) {
		if (glApp->rsmRMax() > 0.001f) {
			glApp->setRSMRMax(glApp->rsmRMax() - 0.001f);
			LOG("RSM R Max = %f\n", glApp->rsmRMax());
			std::vector<GLuint> progs = glApp->programs();

			for (GLuint prog : progs) {
				glUseProgram(prog);
				glUniform1f(glGetUniformLocation(prog, "rsmRMax"), glApp->rsmRMax());
			}
			glUseProgram(0);
		}
	}

	if (keys['k']) {
		glApp->setRSMIntensity(glApp->rsmIntensity() + 0.01f);
		LOG("RSM Intensity = %f\n", glApp->rsmIntensity());
		std::vector<GLuint> progs = glApp->programs();

		for (GLuint prog : progs) {
			glUseProgram(prog);
			glUniform1f(glGetUniformLocation(prog, "rsmIntensity"), glApp->rsmIntensity());
		}
		glUseProgram(0);
	}

	if (keys['j']) {
		if (glApp->rsmIntensity() > 0.01f) {
			glApp->setRSMIntensity(glApp->rsmIntensity() - 0.01f);
			LOG("RSM Intensity = %f\n", glApp->rsmIntensity());
			std::vector<GLuint> progs = glApp->programs();

			for (GLuint prog : progs) {
				glUseProgram(prog);
				glUniform1f(glGetUniformLocation(prog, "rsmIntensity"), glApp->rsmIntensity());
			}
			glUseProgram(0);
		}
	}

	if (moveDir != glm::vec3(0.0f)) {
		glApp->getCamera()->setPosition(glApp->getCamera()->position() + glm::normalize(moveDir) * dt * 10.0f);
		glApp->getCamera()->updateViewMatrix();
	}

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

void rsm::timer(int value) {
	updateFPS(value);
	glutTimerFunc(1000, timer, 0);
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
	glutTimerFunc(0, timer, 0);
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