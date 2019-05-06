#ifndef __RSM_POINTLIGHT_H__
#define __RSM_POINTLIGHT_H__

#include "Light.h"

namespace rsm {

	class PointLight : public Light {
	public:
		PointLight();
		PointLight(const glm::vec3& emission, float intensity);
		PointLight(const glm::vec3& emission, float intensity, const glm::vec3& position);
		PointLight(const glm::vec3& emission, float intensity, const glm::vec3& position, float linearAttenuation, float quadraticAttenuation);

		void toData(LightData& data) const override;
	private:
		float _linearAttenuation;
		float _quadraticAttenuation;
	
	};

}

#endif // !__RSM_POINTLIGHT_H__
