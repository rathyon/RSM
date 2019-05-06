#include "PointLight.h"

using namespace rsm;

PointLight::PointLight() : Light() { }

PointLight::PointLight(const glm::vec3& emission, float intensity)
	: Light(emission, intensity) {
	
	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;
}

PointLight::PointLight(const glm::vec3& emission, float intensity, const glm::vec3& position)
	: Light(emission, intensity, position) {

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;
}

PointLight::PointLight(const glm::vec3& emission, float intensity, const glm::vec3& position, float linear, float quadratic)
	: Light(emission, intensity, position), _linearAttenuation(linear), _quadraticAttenuation(quadratic) { }

void PointLight::toData(LightData& data) const {
	data.position = _position;
	data.direction = glm::vec3(0);
	data.emission = _emission;
	data.intensity = _intensity;
	data.linear = _linearAttenuation;
	data.quadratic = _quadraticAttenuation;
	data.type = LightType::LIGHTYPE_POINT;
	data.state = _on;
	data.cutoff = 0.0f;
}