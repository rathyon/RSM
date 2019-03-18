#include "PointLight.h"

using namespace rsm;

PointLight::PointLight() : Light() { }

PointLight::PointLight(const vec3& emission, float intensity)
	: Light(emission, intensity) { }

PointLight::PointLight(const vec3& emission, float intensity, const vec3& position)
	: Light(emission, intensity, position) { }

void PointLight::toData(LightData& data) const {
	data.position = _position;
	data.emission = _emission;
	data.intensity = _intensity;
	data.type = LightType::LIGHTYPE_POINT;
	data.state = _on;
}