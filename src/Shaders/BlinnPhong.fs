
in FragData {
	vec3 position;
	vec3 normal; 
	vec2 texCoords;
	vec4 lightSpacePosition;
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

uniform cameraBlock {
   	mat4 ViewMatrix;
	mat4 ProjMatrix;
	mat4 ViewProjMatrix;
	vec3 ViewPos;
};

uniform Light lights[NUM_LIGHTS];

//Material parameters
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

// shadow mapping
uniform sampler2D shadowMap;
uniform samplerCube shadowCubeMap;

uniform float far;

float debugShadowFactor(vec3 fragPos, vec3 lightPos){
	vec3 fragToLight = fragPos - lightPos;
	float closestDepth = texture(shadowCubeMap, fragToLight).r;
	return closestDepth;
}

// TODO: Use Bias matrix in vertex shader instead of doing this here, in the frag shader
float debugShadowFactor(vec4 lightSpacePosition, vec3 N, vec3 L){
	// perform perspective divide: clip space-> normalized device coords (done automatically for gl_Position)
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    // bring from [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    return closestDepth;
}


float shadowFactor(vec3 fragPos, vec3 lightPos){
	vec3 fragToLight = fragPos - lightPos;
	float closestDepth = texture(shadowCubeMap, fragToLight).r;
	closestDepth *= far;
	float currentDepth = length(fragToLight);

	float shadow = currentDepth > closestDepth  ? 0.0 : 1.0;
    return shadow;
}

float shadowFactor(vec4 lightSpacePosition, vec3 N, vec3 L){
	// perform perspective divide: clip space-> normalized device coords (done automatically for gl_Position)
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    // bring from [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 1.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z; 

    // shadow bias to reduce shadow acne -> for some reason shadows don't appear?
    /** /
    float bias = max(0.005 * (1.0 - dot(N,L)), 0.0005);
    float shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
    /**/

    float shadow = currentDepth > closestDepth  ? 0.0 : 1.0;
    return shadow;
}

/* ==============================================================================
        Stage Outputs
 ============================================================================== */

out vec4 outColor;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * 0.1 * 1000.0) / (1000.0 + 0.1 - z * (1000.0 - 0.1));
}

void main(void) {

	/** /
	vec3 V = normalize(ViewPos - vsIn.position);
	vec3 N = vsIn.normal;
	vec3 L;

	float shadow = 1.0;

	vec3 retColor = vec3(0.0);

	for(int i=0; i < NUM_LIGHTS; i++){

		// Get Light Vector
		if (lights[i].type == 0){
			L = normalize(-lights[i].direction);

			shadow = shadowFactor(vsIn.lightSpacePosition, N, L);
		}
		else {
			L = normalize(lights[i].position - vsIn.position);

			shadow = shadowFactor(vsIn.position, lights[i].position);
		}

		if (lights[i].type == 2){
			float theta = dot(L, normalize(-lights[i].direction));
			if(theta <= lights[i].cutoff){
				continue;
			}
			shadow = shadowFactor(vsIn.lightSpacePosition, N, L);
		}

		vec3 H = normalize(L + V);

		float NdotL = max(dot(N, L), 0.0);
		float NdotH = max(dot(N, H), 0.0);

		vec3 diff = vec3(0.0);
		vec3 spec = vec3(0.0);

		if (NdotL > 0.0){

			diff = lights[i].emission * lights[i].intensity * ( diffuse * NdotL);
			spec = lights[i].emission * lights[i].intensity * ( specular * pow(NdotH, shininess));

			// if not directional light
			if (lights[i].type != 0){
				float distance = length(lights[i].position - vsIn.position);
				float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * pow(distance, 2.0));

				diff *= attenuation;
				spec *= attenuation;
			}
		}

		retColor += (diff + spec) * shadow;
	}

	outColor = vec4(retColor, 1.0);
	/**/

	// Debug for DirectionalLight shadow mapping
	/** /
	vec3 L = normalize(-lights[0].direction);
	vec3 N = vsIn.normal;
	outColor = vec4(vec3(debugShadowFactor(vsIn.lightSpacePosition, N, L)), 1.0);
	/**/

	// Debug for PointLight shadow mapping
	/**/
	outColor = vec4(vec3(debugShadowFactor(vsIn.position, lights[0].position)), 1.0);
	/**/
}