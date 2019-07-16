#ifndef __RSM_OGLAPP_H__
#define __RSM_OGLAPP_H__

#include "Core/OpenGL.h"
#include "Core/Resources.h"
#include "Core/Scene.h"
#include "Core/Perspective.h"
#include "Core/Model.h"

#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "Lights/SpotLight.h"

#include "Graphics/Shader.h"
#include "Graphics/BlinnPhongMaterial.h"
#include "Graphics/Texture.h"

#include "Math/hammersley.hpp"

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

		Scene getScene();
		sref<Camera> getCamera();
		std::vector<GLuint> programs();

		// RSM
		float rsmRMax();
		float rsmIntensity();

		void setRSMRMax(float val);
		void setRSMIntensity(float val);

	private:
		void prepareCameraBuffer();

		void uploadCameraBuffer();
		void uploadLights(GLuint prog);

		int _width;
		int _height;

		GLuint _cameraBuffer;
		std::vector<GLuint> _programs;

		Scene _scene;
		sref<Camera> _camera;

		// Deferred Shading
		const int _gBufferWidth = 1000;
		const int _gBufferHeight = 1000;
		GLuint _gBuffer;
		GLuint _gPosition, _gNormal, _gDiffuse, _gSpecular, _gLightSpacePosition;

		GLuint _screenQuadVAO;
		GLuint _screenQuadVBO;

		void geometryPass();
		void uploadDeferredShadingData();
		void renderScreenQuad();

		// RSM
		void genRSMaps();
		void uploadShadowMappingData();

		float _rsmRMax;
		float _rsmIntensity;
		GLfloat VPLSamples[NUM_VPL][2];
	};
}

#endif