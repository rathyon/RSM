#include "DirectionalLight.h"

using namespace rsm;

DirectionalLight::DirectionalLight() : Light() { }

DirectionalLight::DirectionalLight(const glm::vec3& emission, float intensity)
	: Light(emission, intensity), _direction(glm::vec3(0.0f, -1.0f, 0.0f)) { }

DirectionalLight::DirectionalLight(const glm::vec3& emission, float intensity, const glm::vec3& direction)
	: Light(emission, intensity), _direction(glm::normalize(direction)) { }

glm::vec3 DirectionalLight::direction() const {
	return _direction;
}

void DirectionalLight::toData(LightData& data) const {
	data.position = _position;
	data.direction = _direction;
	data.emission = _emission;
	data.intensity = _intensity;
	data.linear = 0.0f;
	data.quadratic = 0.0f;
	data.type = LightType::LIGHTYPE_DIR;
	data.state = _on;
	data.cutoff = 0.0f;
}