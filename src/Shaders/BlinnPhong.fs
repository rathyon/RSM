
#define PI 3.1415926538
#define TWO_PI 6.2831853076
#define HALF_PI 1.5707963269

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

uniform float far;
uniform Light lights[NUM_LIGHTS];

//Material parameters
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

// Material textures -> OpenGL ES doesn't let me have 1 shader for both tex and non tex models...
/** /
uniform sampler2D diffuseTex;

vec3 fetchDiffuse(){
	if (diffuse.r < 0.0){
		vec4 texel = texture(diffuseTex, vsIn.texCoords);
		return texel.rgb;
	}
	else{
		return diffuse;
	}
}
/**/

// Shadow Mapping variables
const float baseBias = 0.005f;

// RSM Variables
uniform vec2 VPLSamples[NUM_VPL];
const float rsmRMax = 0.2f;

/* ==============================================================================
        Directional / Spot Lights
 ============================================================================== */
uniform sampler2D depthMap;
uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D fluxMap;

// TODO: Use Bias matrix in vertex shader instead of doing this here, in the frag shader
float debugDepthMap(vec4 lightSpacePosition, vec3 N, vec3 L){
	// perform perspective divide: clip space-> normalized device coords (done automatically for gl_Position)
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    // bring from [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(depthMap, projCoords.xy).r;
    return closestDepth;
}

float shadowFactor(vec4 lightSpacePosition, vec3 N, vec3 L){
	// perform perspective divide: clip space-> normalized device coords (done automatically for gl_Position)
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    // bring from [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 1.0;

    float closestDepth = texture(depthMap, projCoords.xy).r;
    float currentDepth = projCoords.z; 

    float bias = baseBias * tan(acos(dot(N,L)));
    bias = clamp(bias, 0.0, 0.0005f);

    float shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
    return shadow;
}

vec3 debugMap(vec4 lightSpacePosition, vec3 N, vec3 L, sampler2D map) {
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projCoords = projCoords * 0.5 + 0.5;

    vec3 position = texture(map, projCoords.xy).rgb;
    return position;
}

/* ==============================================================================
        Point Lights
 ============================================================================== */

uniform samplerCube depthCubeMap;

float shadowFactor(vec3 fragPos, vec3 lightPos){
	vec3 fragToLight = fragPos - lightPos;
	float closestDepth = texture(depthCubeMap, fragToLight).r;
	closestDepth *= far;
	float currentDepth = length(fragToLight);

	float shadow = currentDepth > closestDepth  ? 0.0 : 1.0;
    return shadow;
}

float debugShadowFactor(vec3 fragPos, vec3 lightPos){
	vec3 fragToLight = fragPos - lightPos;
	float closestDepth = texture(depthCubeMap, fragToLight).r;
	return closestDepth;
}


/* ==============================================================================
        Stage Outputs
 ============================================================================== */

out vec4 outColor;

vec3 directIllumination() {
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

	return retColor;
}

vec3 indirectIllumination() {
	vec3 retColor = vec3(0.0);
	vec3 indirect = vec3(0.0);
	// perform perspective divide: clip space-> normalized device coords (done automatically for gl_Position)

    vec3 projCoords = vsIn.lightSpacePosition.xyz / vsIn.lightSpacePosition.w;

    // bring from [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    for(int i=0; i < NUM_VPL; i++){
    	vec2 rnd = VPLSamples[i];
    	vec2 coords = vec2(projCoords.x + rsmRMax*rnd.x*sin(TWO_PI*rnd.y), projCoords.y + rsmRMax*rnd.x*cos(TWO_PI*rnd.y));

    	vec3 vplP = texture(positionMap, coords.xy).xyz;
    	vec3 vplN = normalize(texture(normalMap, coords.xy)).xyz;
    	vec3 vplFlux = texture(fluxMap, coords.xy).rgb;

    	float dot1 = max(0.0, dot(vplN, vsIn.position - vplP));
    	float dot2 = max(0.0, dot(normalize(vsIn.normal), vplP - vsIn.position));

    	float dist = length(vplP - vsIn.position);

    	indirect += vplFlux * (dot1 * dot2) / (dist * dist * dist * dist);
    	indirect = indirect * rnd.x * rnd.x;

    	retColor += indirect;
    }
	return clamp(retColor, 0.0, 1.0) * diffuse;
}

void main(void) {

	outColor = vec4( directIllumination() + indirectIllumination(), 1.0);
	//outColor = vec4( directIllumination(), 1.0);
	//outColor = vec4( indirectIllumination(), 1.0);
	/**/

	// Debug for DirectionalLight shadow mapping
	//outColor = vec4(vsIn.position, 1.0);

	/** /
	vec3 L = normalize(-lights[0].direction);
	vec3 N = vsIn.normal;
	outColor = vec4(vec3(debugMap(vsIn.lightSpacePosition, N, L, normalMap)), 1.0);
	//outColor = vec4(vec3(debugDepthMap(vsIn.lightSpacePosition, N, L)), 1.0);
	/**/

	// Debug for PointLight shadow mapping
	/** /
	outColor = vec4(vec3(debugShadowFactor(vsIn.position, lights[0].position)), 1.0);
	/**/
	
}