#include <OpenGLApplication.h>

#include <string>

namespace rsm {

	void init(int argc, char* argv[]);
	void loop(); // glutMainLoop
	void setupCallbacks();
	void updateFPS(int value); //ignore "value"
	void timer(int value); // ignore "value"
	void setTitle(const std::string& title);

	void update(); // time dependent updates
	void render();
	void idle();
	void reshape(int w, int h);

	// input related funcs
	void updateMouse(int x, int y);
	void mouseMotion(int x, int y);
	void keyPress(unsigned char key, int x, int y);
	void keyUp(unsigned char key, int x, int y);
	void mouseClick(int button, int state, int x, int y);

	void cleanup();

	int windowHandle;
	int width, height;
	std::string title;

	int mouseX, mouseY;
	float mouseDx, mouseDy;
	bool keys[255];

	int frameCount;
	int oldTimeSinceStart;

	OpenGLApplication* glApp;
}