#ifndef __RSM_OGLAPP_H__
#define __RSM_OGLAPP_H__

#include "Core/OpenGL.h"

#include "Resources.h"

#include "Scene.h"

#include "Perspective.h"

#include "Model.h"

#include "PointLight.h"

#include "Shader.h"

#include "BlinnPhongMaterial.h"

namespace rsm {

	class OpenGLApplication {
	public:
		OpenGLApplication(int width, int height);

		void init();
		void prepare();
		void cleanup();

		void reshape(int w, int h);

		void update(float dt);
		void render(); // receive scene and camera args

		int getWidth();
		int getHeight();

	private:
		int _width;
		int _height;

		Scene _scene;
	};
}

#endif