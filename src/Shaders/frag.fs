
in FragData {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
} vsIn;

struct Light {
	vec3 position;
	vec3 emission;
	float intensity;
	int type;
	bool state; 
};

uniform cameraBlock {
   	mat4 ViewMatrix;
	mat4 ProjMatrix;
	mat4 ViewProjMatrix;
	vec3 ViewPos;
};

const int NUM_LIGHTS = 1;

uniform lightBlock {
	Light lights[NUM_LIGHTS];
};


//Material parameters
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

out vec4 outColor;

void main(void) {

	outColor = vec4(diffuse, 1.0);
}