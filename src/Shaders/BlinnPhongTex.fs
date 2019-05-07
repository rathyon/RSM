const int NUM_LIGHTS = 1;
// Passes everything in world coordinates to the fragment shader
in FragData {
	vec3 position;
	vec2 texCoords;
	vec3 lightPos[NUM_LIGHTS];
	vec3 lightDir[NUM_LIGHTS];
	vec3 viewPos;
} vsIn;

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
uniform float shininess;

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalMap;

// for specular maps
float fetchParameter(sampler2D samp){
	return texture(samp, vsIn.texCoords).r;
}

vec3 fetchNormal(){
	// normal map [0,1] to [-1,1]
	return normalize((texture(normalMap, vsIn.texCoords).rgb) * 2.0 - 1.0);
}

vec3 fetchDiffuse(){
	vec4 texel = texture(diffuseTex, vsIn.texCoords);
	if (texel.a <= 0.0)
		discard;
	return texel.rgb;
}

/* ==============================================================================
        Stage Outputs
 ============================================================================== */

out vec4 outColor;

void main(void) {

	/**/
	vec3 V = normalize(vsIn.viewPos - vsIn.position);
	vec3 N = fetchNormal();
	vec3 L;

	vec3 retColor = vec3(0);

	for(int i=0; i < NUM_LIGHTS; i++){

		if (lights[i].type == 0){
			L = normalize(-vsIn.lightDir[i]);
		}
		else {
			L = normalize(vsIn.lightPos[i] - vsIn.position);
		}

		if (lights[i].type == 2){
			float theta = dot(L, normalize(-vsIn.lightDir[i]));
			if(theta <= lights[i].cutoff){
				continue;
			}
		}

		vec3 H = normalize(L + V);

		float NdotL = max(dot(N, L), 0.0);
		float NdotH = max(dot(N, H), 0.0);

		vec3 diff = vec3(0);
		vec3 spec = vec3(0);

		if (NdotL > 0){

			diff = lights[i].emission * lights[i].intensity * ( fetchDiffuse() * NdotL);
			spec = lights[i].emission * lights[i].intensity * ( fetchParameter(specularTex) * pow(NdotH, shininess));

			// if not directional light
			if (lights[i].type != 0){
				float distance = length(vsIn.lightPos[i] - vsIn.position);
				float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * pow(distance, 2));

				diff *= attenuation;
				spec *= attenuation;
			}
		}

		retColor += diff + spec;
	}
	/**/

	//outColor = vec4(fetchNormal(), 1.0);
}