#include "SpotLight.h"

using namespace rsm;

SpotLight::SpotLight() : Light() { }

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff)
	: Light(emission, intensity) {

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;

	_direction = glm::vec3(0.0f, -1.0f, 0.0f);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff, const glm::vec3& direction)
	: Light(emission, intensity) { 

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;

	_direction = glm::normalize(direction);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff, const glm::vec3& direction, const glm::vec3& position)
	: Light(emission, intensity, position) {

	// approximate values for MOST cases
	_linearAttenuation = 0.1f;
	_quadraticAttenuation = 0.032;

	_direction = glm::normalize(direction);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff, const glm::vec3& direction, const glm::vec3& position, float linearAttenuation, float quadraticAttenuation) {
	// approximate values for MOST cases
	_linearAttenuation = linearAttenuation;
	_quadraticAttenuation = quadraticAttenuation;

	_direction = glm::normalize(direction);
	_cutoff = glm::cos(glm::radians(cutoff));
}

glm::vec3 SpotLight::direction() const {
	return _direction;
}

float SpotLight::cutoff() const {
	return _cutoff;
}

void SpotLight::toData(LightData& data) const {
	data.position = _position;
	data.direction = _direction;
	data.emission = _emission;
	data.intensity = _intensity;
	data.linear = _linearAttenuation;
	data.quadratic = _quadraticAttenuation;
	data.type = LightType::LIGHTYPE_SPOT;
	data.state = _on;
	data.cutoff = _cutoff;
}