#ifndef __RSM_DIRECTIONALLIGHT_H__
#define __RSM_DIRECTIONALLIGHT_H__

#include "Light.h"
#include "Graphics/Texture.h"

namespace rsm {

	class DirectionalLight : public Light {
	public:
		DirectionalLight();
		DirectionalLight(const glm::vec3& emission);
		DirectionalLight(const glm::vec3& emission, const glm::vec3& direction);

		glm::vec3 direction() const;

		void toData(LightData& data) const override;

		// shadow mapping
		GLenum depthMapType() override;
		void prepare(int width, int height, float orthoDim, float nearP, float farP, glm::vec3 eye, glm::vec3 at);
		void uploadSpatialData(GLuint program) override;
		void uploadShadowMapData(GLuint program) override;

		void updateMatrix() override;

	private:
		glm::vec3 _direction;

		// shadow mapping
		glm::mat4 _viewMatrix;
		glm::mat4 _viewProjMatrix;
	};

}

#endif // !__RSM_POINTLIGHT_H__
