#ifndef __RSM_POINTLIGHT_H__
#define __RSM_POINTLIGHT_H__

#include "Light.h"
#include "Graphics/Image.h"

namespace rsm {

	class PointLight : public Light {
	public:
		PointLight();
		PointLight(const glm::vec3& emission);
		PointLight(const glm::vec3& emission, const glm::vec3& position);
		PointLight(const glm::vec3& emission, const glm::vec3& position, float linearAttenuation, float quadraticAttenuation);

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

		// shadow mapping
		float _far;
		// precomputed V * P matrices
		std::vector<glm::mat4> _viewProjMatrices;
	};

}

#endif // !__RSM_POINTLIGHT_H__
