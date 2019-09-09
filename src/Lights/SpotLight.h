#ifndef __RSM_SPOTLIGHT_H__
#define __RSM_SPOTLIGHT_H__

#include "Light.h"
#include "Graphics/Texture.h"

namespace rsm {

	class SpotLight : public Light {
	public:
		SpotLight();
		SpotLight(const glm::vec3& emission, float cutoff);
		SpotLight(const glm::vec3& emission, float cutoff, const glm::vec3& direction);
		SpotLight(const glm::vec3& emission, float cutoff, const glm::vec3& direction, const glm::vec3& position);
		SpotLight(const glm::vec3& emission, float cutoff, const glm::vec3& direction, const glm::vec3& position, float linearAttenuation, float quadraticAttenuation);

		glm::vec3 direction() const;
		float cutoff() const;

		void toData(LightData& data) const override;

		// shadow mapping
		GLenum depthMapType() override;
		void prepare(int width, int height);
		void uploadSpatialData(GLuint program) override;
		void uploadShadowMapData(GLuint program) override;

		void updateMatrix() override;

	private:
		float _linearAttenuation;
		float _quadraticAttenuation;

		glm::vec3 _direction;
		// cutoff is passed in constructor in degrees
		// but it is stored as cosine(cutoff)
		float _cutoff;

		// shadow mapping
		glm::mat4 _viewMatrix;
		glm::mat4 _viewProjMatrix;
	};

}

#endif // !__RSM_POINTLIGHT_H__
