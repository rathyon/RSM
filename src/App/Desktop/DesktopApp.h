#include <OpenGLApplication.h>

#include <string>

namespace rsm {

	void init(int argc, char* argv[]);
	void loop(); // glutMainLoop
	void setupCallbacks();
	void updateFPS(int value); //ignore "value"
	void setTitle(const std::string& title);

	void update(); // time dependent updates
	void render();
	void idle();
	void reshape(int w, int h);

	void cleanup();

	int windowHandle;
	int width, height;
	std::string title;
	int frameCount;

	int oldTimeSinceStart;

	OpenGLApplication* glApp;
}