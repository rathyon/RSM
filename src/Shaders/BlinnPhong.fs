
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

uniform Light lights[NUM_LIGHTS];

//Material parameters
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalTex;

/* ==============================================================================
        Stage Outputs
 ============================================================================== */

out vec4 outColor;

// for specular maps
float fetchParameter(sampler2D samp, float val){
	if(val > 0.0){
		return val;
	}
	else{
		return texture(samp, vsIn.texCoords).r;
	}
}

vec3 fetchDiffuse(){
	if(diffuse.r > 0.0){
		return diffuse;
	}
	else{
		return texture(diffuseTex, vsIn.texCoords).rgb;
	}
}

void main(void) {

	/**/
	vec3 N = normalize(vsIn.normal);
	vec3 V = normalize(ViewPos - vsIn.position);
	vec3 L;

	vec3 retColor = ambient;
	for(int i=0; i < NUM_LIGHTS; i++){
		// if its a directional light
		if(lights[i].type == 0){
			L = normalize(-lights[i].position);
		}
		else{
			L = normalize(lights[i].position - vsIn.position);
		}
		vec3 H = normalize(L + V);

		float NdotL = dot(N, L);
		float NdotH = dot(N, H);

		vec3 diff = lights[i].emission * fetchDiffuse() * NdotL;
		vec3 spec = lights[i].emission * fetchParameter(specularTex, specular.r) * pow(NdotH, shininess);

		retColor += diff + spec;
	}

	outColor = vec4(retColor, 1.0);
	/**/

	//outColor = vec4(vec3(fetchParameter(specularTex, specular.r)), 1.0);
}