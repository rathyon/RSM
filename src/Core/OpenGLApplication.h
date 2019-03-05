#ifndef __RSM_OGLAPP_H__
#define __RSM_OGLAPP_H__

#include "OpenGL.h"

#include "Resources.h"
#include "../Graphics/Shader.h"

namespace rsm {

	class OpenGLApplication {
	public:
		OpenGLApplication();

		void init();
		void prepare();
		void cleanup();

		void reshape(int w, int h);

		void update(float dt);
		void render(); // receive scene and camera args

		int getWidth();
		int getHeight();

	protected:
		int _width;
		int _height;
	};
}

#endif