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
				Conference
	=====================================================================================*/

	/** /
	sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-25.0f, -15.0f, 0.0f));
	glApp->getScene()->addLight(sun);
	sun->prepare(width, height, 15.0f, 0.1f, 10000.0f, glm::vec3(20.0f, 16.f, -3.f), glm::vec3(-5.0f, 1.0f, -3.0f));
	/**/

	/* ===================================================================================
				Sphere and Cube
	=====================================================================================*/

	/** /
	sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(10.0f, -7.5f, -18.0f));
	glApp->getScene()->addLight(sun);
	sun->prepare(width, height, 10.0f, 0.1f, 1000.0f, glm::vec3(-7.0f, 10.f, 15.f), glm::vec3(3.0f, 2.5f, -3.0f));
	/**/

	/* ===================================================================================
				Lucy
	=====================================================================================*/

	/**/
	sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, -1.0f));
	glApp->getScene()->addLight(sun);
	sun->prepare(width, height, 10.0f, 0.1f, 1000.0f, glm::vec3(10.f, 10.f, 10.f), glm::vec3(0.f, 0.f, 0.f));
	/**/

	/* ===================================================================================
				Crytek Sponza
	=====================================================================================*/

	/** /
	sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-89.5f, -85.0f, -15.0f));
	glApp->getScene()->addLight(sun);
	sun->prepare(width, height, 100.0f, 0.1f, 100000.0f, glm::vec3(50.f, 85.f, 5.f), glm::vec3(-39.5f, 0.f, -10.f));
	/**/

	/* ===================================================================================
				Cornell Box
	=====================================================================================*/

	/** /
	sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.5f, -0.4f, -1.0f));
	glApp->getScene()->addLight(sun);
	sun->prepare(width, height, 10.0f, 0.1f, 1000.0f, glm::vec3(5.f, 4.f, 10.f), glm::vec3(0.f, 0.f, 0.f));
	/**/

	/* ===================================================================================
				Sibenik
	=====================================================================================*/

	/** /
	sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, -1.0f));
	glApp->getScene()->addLight(sun);
	sun->prepare(width, height, 10.0f, 0.1f, 10000.0f, glm::vec3(-10.f, 10.f, 0.f), glm::vec3(15.f, 0.f, 0.f));
	/**/

	/* ===================================================================================
				Unused...
	=====================================================================================*/
	/** /
	// emission, cutoff, direction, position
	sref<SpotLight> spot = make_sref<SpotLight>(glm::vec3(2.0f, 2.0f, 2.0f), 20.0f, glm::vec3(-1.0f, -0.5f, -1.0f), glm::vec3(8.0f, 8.0f, 8.0f));
	_scene.addLight(spot);
	spot->prepare(_width, _height);
	/**/

	const sref<Light>& light = glApp->getScene()->lights()[0];
	LightData data;
	light->toData(data);
	const int lightType = data.type;

	/* ===================================================================================
				Shaders
	=====================================================================================*/
	/**/

	{
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
	}

	/* ===================================================================================
				Test Scenes
	=====================================================================================*/

	/* ===================================================================================
				Conference
	=====================================================================================*/

	/** /
	sref<Model> Conference = make_sref<Model>("Conference");
	Conference->loadFromFile("../../../assets/models/Conference Modified/conference.obj", "../../../assets/models/Conference Modified/");
	RM.addModel("Conference", Conference);
	Conference->prepare();
	glApp->getScene()->addModel(Conference);

	sref<Camera> camera = make_sref<Perspective>(width, height,
		vec3(6.5f, 4.0f, -10.0f),
		vec3(-5.0f, 2.0f, -1.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 1000.0f, 60.0f);

	glApp->setCamera(camera);
	glApp->getScene()->addCamera(camera);
	/**/

	/* ===================================================================================
				Sphere and Cube
	=====================================================================================*/

	/** /
	sref<Model> SphereCube = make_sref<Model>("SphereCube");
	SphereCube->loadFromFile("../../../assets/models/Sphere and Cube/spherecube.obj", "../../../assets/models/Sphere and Cube/");
	RM.addModel("SphereCube", SphereCube);
	SphereCube->prepare();
	glApp->getScene()->addModel(SphereCube);

	sref<Camera> camera = make_sref<Perspective>(width, height,
		vec3(0.0f, 5.0f, 12.0f),
		vec3(0.0f, 2.5f, -1.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 1000.0f, 60.0f);

	glApp->setCamera(camera);
	glApp->getScene()->addCamera(camera);
	/**/

	/* ===================================================================================
				Lucy
	=====================================================================================*/

	/**/
	sref<Model> Lucy = make_sref<Model>("Lucy");
	Lucy->loadFromFile("../../../assets/models/Lucy/Lucy.obj", "../../../assets/models/Lucy/");
	RM.addModel("Lucy", Lucy);
	Lucy->prepare();
	glApp->getScene()->addModel(Lucy);

	sref<Camera> camera = make_sref<Perspective>(width, height,
		vec3(7.75f, 5.0f, 2.0f),
		vec3(-4.6f, 2.4f, -2.2f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 1000.0f, 60.0f);

	glApp->setCamera(camera);
	glApp->getScene()->addCamera(camera);
	/**/

	/* ===================================================================================
				Crytek Sponza
	=====================================================================================*/

	/** /
	sref<Model> sponza = make_sref<Model>("sponza");
	sponza->loadFromFile("../../../assets/models/crytek sponza/sponza.obj", "../../../assets/models/crytek sponza/");
	RM.addModel("sponza", sponza);
	sponza->prepare();
	sponza->setScale(0.05f, 0.05f, 0.05f);
	glApp->getScene()->addModel(sponza);

	sref<Camera> camera = make_sref<Perspective>(width, height,
		vec3(2.0f, 11.0f, -2.0f),
		vec3(-71.0f, 14.0f, -4.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 100000.0f, 60.0f);

	glApp->setCamera(camera);
	glApp->getScene()->addCamera(camera);
	/**/

	/* ===================================================================================
				Cornell Box
	=====================================================================================*/

	/** /
	sref<Model> CB = make_sref<Model>("CB");
	CB->loadFromFile("../../../assets/models/CornellBox/CornellBox-Original.obj", "../../../assets/models/CornellBox/");
	RM.addModel("CB", CB);
	CB->prepare();
	CB->setScale(3.0f, 3.0f, 3.0f);
	glApp->getScene()->addModel(CB);

	sref<Camera> camera = make_sref<Perspective>(width, height,
		vec3(0.0f, 3.0f, 6.0f),
		vec3(0.0f, 3.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 1000.0f, 60.0f);

	glApp->setCamera(camera);
	glApp->getScene()->addCamera(camera);
	/**/

	/* ===================================================================================
				Sibenik
	=====================================================================================*/

	/** /
	sref<Model> sibenik = make_sref<Model>("sibenik");
	sibenik->loadFromFile("../../../assets/models/Sibenik Modified/sibenik.obj", "../../../assets/models/Sibenik Modified/");
	RM.addModel("sibenik", sibenik);
	sibenik->prepare();
	//sibenik->setScale(0.05f, 0.05f, 0.05f);
	glApp->getScene()->addModel(sibenik);

	sref<Camera> camera = make_sref<Perspective>(width, height,
		vec3(-2.0f, 5.0f, 1.5f),
		vec3(13.0f, 2.0f, -1.0f),
		vec3(0.0f, 1.0f, 0.0f),
		0.1f, 100000.0f, 60.0f);

	glApp->setCamera(camera);
	glApp->getScene()->addCamera(camera);
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