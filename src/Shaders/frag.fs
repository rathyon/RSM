
// Passes everything in world coordinates to the fragment shader
in FragData {
	vec3 position;
	vec3 normal; 
	vec2 texCoords;
} vsIn;

struct Light {
	//TODO 
}

const int NUM_LIGHTS = 4;

uniform lightBlock {
	Light lights[NUM_LIGHTS];
}

//Material parameters
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;


uniform cameraBlock {
   	mat4 ViewMatrix;
	mat4 ProjMatrix;
	mat4 ViewProjMatrix;
	vec3 ViewPos;
};

out vec4 outColor;

// Import common functions
// TODO? ...


void main(void) {
	vec3 N = normalize(vsIn.normal);
	vec3 L = normalize(vec3(0, 1, 1));
	vec3 V = normalize(ViewPos - vsIn.position);

	vec3 H = normalize(L + V);

	float NdotL = dot(N, L);
	float NdotH = dot(N, H);

	vec3 diff = vec3(0, 0, 0);
	vec3 spec = vec3(0, 0, 0);
	if (NdotL > 0) {
		diff = diffuse * NdotL;
		spec =specular * pow(NdotH, shininess);
	}

	vec3 retColor = ambient + diff + spec;

	outColor = vec4(retColor, 1.0);
}