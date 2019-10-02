
in FragData {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
} vsIn;

struct Light {
	vec3 position;
	vec3 direction;
	vec3 emission;
	float linear;
	float quadratic;
	int type;
	bool state;
	float cutoff;
};

uniform Light light;

//Material parameters
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

uniform sampler2D diffuseTex;

vec3 fetchDiffuse(){
	if (diffuse.r < 0.0){
		return texture(diffuseTex, vsIn.texCoords).rgb;
	}
	else{
		return diffuse;
	}
}

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 flux;

void main(void) {
	position = vsIn.position;
	//normal = vsIn.normal * 0.5 + 0.5;
	normal = vsIn.normal;
	flux = light.emission * fetchDiffuse();
}