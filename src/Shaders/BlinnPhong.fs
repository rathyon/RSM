
// Passes everything in world coordinates to the fragment shader
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

	/** /
	vec3 N = normalize(vsIn.normal);
	// temporary directional light for testing
	vec3 L = normalize(vec3(1,1,0));
	vec3 V = normalize(ViewPos - vsIn.position);

	vec3 H = normalize(L + V);

	float NdotL = dot(N, L);
	float NdotH = dot(N, H);

	vec3 diff = lightRGB * diffuse * NdotL;
	vec3 spec = lightRGB * specular * pow(NdotH, shininess);

	vec3 retColor = ambient + diff + spec;

	outColor = vec4(vec3(lightRGB), 1.0);

	/**/
	/** /
	vec4 lightRGB = lights[0].emission;
	outColor = lightRGB;
	/**/
	vec3 N = normalize(vsIn.normal);
	outColor = vec4(N, 1.0);
}