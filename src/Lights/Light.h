#ifndef __RSM_LIGHT_H__
#define __RSM_LIGHT_H__

#include "Core\OpenGL.h"
#include "Core\SceneObject.h"

namespace rsm {

	// Used to pass the type id to shaders
	enum LightType : int {
		LIGHTYPE_DIR   = 0,
		LIGHTYPE_POINT = 1,
		LIGHTYPE_SPOT  = 2,
	};

	// Light data for shader blocks
	// CARE: data is properly aligned, do not change
	struct LightData {
		glm::vec3 position;
		glm::vec3 direction;   // for directional and spot lights
		glm::vec3 emission;    // "color" of the light
		float intensity;
		float linear;	 // attenuation
		float quadratic; // attenuation
		int type;
		bool state;       // on or off
		float cutoff;
	};

	class Light : public SceneObject {
	public:
		Light();
		Light(const glm::vec3& emission, float intensity);
		Light(const glm::vec3& emission, float intensity, const glm::vec3& position);
		//Light(const vec3& emission, float intensity, const mat4& lightToWorld);

		bool isOn() const;
		bool castingShadows() const;
		float intensity() const;
		glm::vec3 emission() const;

		void setOn(bool on);
		void setCastShadows(bool val);
		void setIntensity(float intensity);
		void setEmission(glm::vec3& emission);

		virtual void toData(LightData& data) const = 0;

		// shadow mapping
		glm::mat4 projMatrix() const;
		glm::mat4 viewMatrix() const;
		glm::mat4 viewProjMatrix() const;

		int resolution() const;
		GLuint depthMapFBO() const;
		GLuint depthMap() const;

		virtual void prepare(int resolution) = 0;

	protected:
		bool _on;
		bool _shadows;
		float _intensity;
		glm::vec3 _emission;

		// shadow mapping
		int _resolution;
		GLuint _depthMapFBO;
		GLuint _depthMap;

		glm::mat4 _projMatrix;
		glm::mat4 _viewMatrix;
		//glm::mat4 _viewProjMatrix; 
	};

}

#endif // !__RSM_LIGHT_H__
