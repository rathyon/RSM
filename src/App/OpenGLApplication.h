#ifndef __RSM_OGLAPP_H__
#define __RSM_OGLAPP_H__

#include "Core\OpenGL.h"
#include "Core\Resources.h"
#include "Core\Scene.h"
#include "Core\Perspective.h"
#include "Core\Model.h"

#include "Lights\PointLight.h"

#include "Graphics\Shader.h"
#include "Graphics\BlinnPhongMaterial.h"
#include "Graphics\Texture.h"

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

		void addProgram(GLuint prog);

		int getWidth();
		int getHeight();

	private:
		void prepareCameraBuffer();

		void uploadCameraBuffer();

		void uploadLights();

		int _width;
		int _height;

		GLuint _cameraBuffer;
		std::vector<GLuint> _programs;

		Scene _scene;
		sref<Camera> _camera;
	};
}

#endif