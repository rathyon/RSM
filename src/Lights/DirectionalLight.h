#ifndef __RSM_DIRECTIONALLIGHT_H__
#define __RSM_DIRECTIONALLIGHT_H__

#include "Light.h"
#include "Graphics\Texture.h"

namespace rsm {

	class DirectionalLight : public Light {
	public:
		DirectionalLight();
		DirectionalLight(const glm::vec3& emission, float intensity);
		DirectionalLight(const glm::vec3& emission, float intensity, const glm::vec3& direction);

		glm::vec3 direction() const;

		void toData(LightData& data) const override;

		// shadow mapping
		void prepare(int resolution) override;

	private:
		glm::vec3 _direction;
	};

}

#endif // !__RSM_POINTLIGHT_H__
