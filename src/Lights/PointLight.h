#ifndef __RSM_POINTLIGHT_H__
#define __RSM_POINTLIGHT_H__

#include "Light.h"

namespace rsm {

	class PointLight : public Light {
	public:
		PointLight();
		PointLight(const vec3& emission, float intensity);
		PointLight(const vec3& emission, float intensity, const vec3& position);

		void toData(LightData& data) const override;
	
	};

}

#endif // !__RSM_POINTLIGHT_H__
