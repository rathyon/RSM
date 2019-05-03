#include "SpotLight.h"

using namespace rsm;

SpotLight::SpotLight() : Light() { }

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff)
	: Light(emission, intensity) {

	_direction = glm::vec3(0.0f, -1.0f, 0.0f);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff, const glm::vec3& direction)
	: Light(emission, intensity) { 

	_direction = glm::normalize(direction);
	_cutoff = glm::cos(glm::radians(cutoff));
}

SpotLight::SpotLight(const glm::vec3& emission, float intensity, float cutoff, const glm::vec3& direction, const glm::vec3& position)
	: Light(emission, intensity, position) {

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
	data.type = LightType::LIGHTYPE_SPOT;
	data.state = _on;
	data.cutoff = _cutoff;
}