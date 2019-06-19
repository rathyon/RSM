
in FragData {
	vec3 position;
	vec3 normal;
} vsIn;

// depth = 0
// position = 1
// normal = 2
// flux = 3

struct Light {
	vec3 position;
	vec3 direction;
	vec3 emission;
	float intensity;
	float linear;
	float quadratic;
	int type;
	bool state;
	float cutoff;
};

uniform Light lights[NUM_LIGHTS];

//Material parameters
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

uniform int mode;

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 flux;

void main(void) {

	position = vec4(vsIn.position, 1.0);
	normal = vec4(vsIn.normal, 1.0);
	flux = vec4(lights[0].emission * diffuse, 1.0);
}