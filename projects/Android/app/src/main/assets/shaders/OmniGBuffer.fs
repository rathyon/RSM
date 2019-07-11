
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

uniform Light lights[NUM_LIGHTS];
uniform int lightIdx;
uniform float far;

//Material parameters
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

uniform sampler2D diffuseTex;

vec3 fetchDiffuse(){
	if (diffuse.r < 0.0){
			vec4 texel = texture(diffuseTex, vsIn.texCoords);
		if (texel.a <= 0.0)
			discard;
		return texel.rgb;
	}
	else{
		return diffuse;
	}
}

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 flux;

void main(void) {
	// get distance between fragment and light source
    float dist = length(vsIn.position.xyz - lights[lightIdx].position);
    // map to [0;1] range by dividing by far
    dist = dist / far;
    // write this as modified depth
    gl_FragDepth = dist;

	position = vec4(vsIn.position, 1.0);
	normal = vec4(normalize(vsIn.normal), 1.0);
	flux = vec4(lights[lightIdx].emission * fetchDiffuse(), 1.0);
}