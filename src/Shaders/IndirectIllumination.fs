
#define PI 3.1415926538
#define TWO_PI 6.2831853076
#define HALF_PI 1.5707963269

in vec2 texCoords;

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

uniform cameraBlock {
   	mat4 ViewMatrix;
	mat4 ProjMatrix;
	mat4 ViewProjMatrix;
	vec3 ViewPos;
};

uniform Light light;

// Shadow filtering
const float baseBias = 0.005f;

// RSM Variables
uniform vec2 VPLSamples[NUM_VPL];
uniform float VPLWeights[NUM_VPL];
uniform vec2 VPLCoords[NUM_VPL];
uniform float rsmRMax;
uniform float rsmIntensity;

/* ==============================================================================
        Deferred Shading
 ============================================================================== */
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gLightSpacePosition;

// assuming global value for shininess for now...
const float shininess = 16.0f;

/* ==============================================================================
        Directional / Spot Lights
 ============================================================================== */
uniform sampler2D depthMap;
uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D fluxMap;
/* ==============================================================================
        Illumination
 ============================================================================== */
 vec3 projCoords;

vec3 indirectIllumination(vec3 FragPos, vec3 LightSpacePos, vec3 Normal, vec3 Diffuse) {
	vec3 result = vec3(0.0);
	vec3 indirect = vec3(0.0);
	// perform perspective divide: clip space-> normalized device coords (done automatically for gl_Position)

    //vec3 projCoords = LightSpacePos.xyz / LightSpacePos.w;

    // bring from [-1,1] to [0,1]
    //projCoords = projCoords * 0.5 + 0.5;

    for(int i=0; i < NUM_VPL; i++){
    	vec2 rnd = VPLSamples[i];
    	//vec2 coords = vec2(projCoords.x + rsmRMax*rnd.x*sin(TWO_PI*rnd.y), projCoords.y + rsmRMax*rnd.x*cos(TWO_PI*rnd.y));
        vec2 coords = vec2(projCoords.x + VPLCoords[i].x, projCoords.y + VPLCoords[i].y);

    	vec3 vplP = texture(positionMap, coords.xy).xyz;
        vec3 vplN = texture(normalMap, coords.xy).xyz * 2.0 - 1.0;
        //vec3 vplN = texture(normalMap, coords.xy).xyz;

        float dot2 = max(0.0, dot(Normal, vplP - FragPos));

        vec3 vplFlux = texture(fluxMap, coords.xy).rgb;

        float dist = length(vplP - FragPos);
        float dot1 = max(0.0, dot(vplN, FragPos - vplP));

        // frag == vpl pos???
        //if(dist <= 0.0)
        //    continue;

        indirect = (vplFlux * (dot1 * dot2) / (dist * dist * dist * dist)) * VPLWeights[i];

        //indirect = indirect * VPLWeights[i];
        //indirect = indirect * (1.0 / float(NUM_VPL));
    	result += indirect;
    }
    //result = result * (1.0 / float(NUM_VPL));
	return (result * Diffuse) * rsmIntensity;
}

/* ==============================================================================
        Stage Outputs
 ============================================================================== */

layout(location = 0) out vec4 outColor;

void main(void) {

	vec3 pos      = texture(gPosition, texCoords).rgb;
	vec3 N        = texture(gNormal, texCoords).rgb * 2.0 - 1.0;
    //vec3 N        = texture(gNormal, texCoords).rgb;
	vec3 diffuse  = texture(gDiffuse, texCoords).rgb;
	vec3 lightSpacePos = texture(gLightSpacePosition, texCoords).rgb;
    projCoords = lightSpacePos;

	outColor = vec4(indirectIllumination(pos, lightSpacePos, N, diffuse), 1.0);

}