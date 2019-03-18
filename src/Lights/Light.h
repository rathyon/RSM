#ifndef __RSM_LIGHT_H__
#define __RSM_LIGHT_H__

#include "SceneObject.h"

namespace rsm {

	// Used to pass the type id to shaders
	enum LightType : int {
		LIGHTYPE_POINT = 0,
		LIGHTYPE_SPOT  = 1,
		LIGHTYPE_DIR   = 2,
	};

	// Light data for shader blocks
	// CARE: data is properly aligned, do not change
	struct LightData {
		vec3 position;
		vec3 emission;    // "color" of the light
		float intensity;
		int type;
		bool state;       // on or off
	};

	class Light : public SceneObject {
	public:
		Light();
		Light(const vec3& emission, float intensity);
		Light(const vec3& emission, float intensity, const vec3& position);
		//Light(const vec3& emission, float intensity, const mat4& lightToWorld);

		bool isOn() const;
		bool castingShadows() const;
		float intensity() const;
		vec3 emission() const;

		void setOn(bool on);
		void setCastShadows(bool val);
		void setIntensity(float intensity);
		void setEmission(vec3& emission);

		virtual void toData(LightData& data) const = 0;

	protected:
		bool _on;
		bool _shadows;
		float _intensity;
		vec3 _emission;
	};

}

#endif // !__RSM_LIGHT_H__
