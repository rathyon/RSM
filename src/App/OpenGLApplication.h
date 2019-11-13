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
		OpenGLApplication(int width, int height, int rsm_version, int num_vpl, int indirect_width, int indirect_height);

		void init();
		void prepare();
		void cleanup();

		void reshape(int w, int h);

		void update(float dt);
		void render(); // receive scene and camera args

		void addProgram(GLuint prog);

		int getWidth();
		int getHeight();

		Scene* getScene();
		sref<Camera> getCamera();
		void setCamera(const sref<Camera>& camera);
		std::vector<GLuint> programs();

		// RSM
		float rsmRMax();
		float rsmIntensity();

		void setRSMRMax(float val);
		void setRSMIntensity(float val);
		void setGlobalSpecular(float r, float g, float b);
		void setGlobalShininess(float globalShininess);

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

		// State variables (too much effort to actually use State Pattern...)
		// 0 = Forward; 1 = Deferred; 2 = Accelerated Deferred
		int _rsm_version;
		int _num_vpl;
		int _indirect_width, _indirect_height;
		glm::vec3 _globalSpecular;
		float _globalShininess;


		// Deferred Shading
		int _gBufferWidth;
		int _gBufferHeight;
		GLuint _gBuffer;
		GLuint _gPosition, _gNormal, _gDiffuse, _gLightSpacePosition;

		GLuint _screenQuadVAO;
		GLuint _screenQuadVBO;

		void prepareDeferredShading();
		void geometryPass();
		void uploadDeferredShadingData();
		void renderScreenQuad();

		// RSM
		float _rsmRMax;
		float _rsmIntensity;
		//GLfloat VPLSamples[NUM_VPL][2];
		//GLfloat VPLWeights[NUM_VPL];
		//GLfloat VPLCoords[NUM_VPL][2];
		std::vector< std::vector<GLfloat>> _VPLSamples;
		std::vector<GLfloat> _VPLWeights;
		std::vector< std::vector<GLfloat>> _VPLCoords;

		int _indirectLowResWidth;
		int _indirectLowResHeight;
		GLuint _indirectLowResFBO;
		GLuint _indirectLowResMap;

		glm::vec4 _indirectSampleParams;

		void prepareRSM();
		void genRSMaps();
		void uploadShadowMappingData();
		void renderLowResIndirect();
		void uploadLowResIndirect();
	};
}

#endif