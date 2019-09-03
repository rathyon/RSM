
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
uniform float rsmRMax;
uniform float rsmIntensity;

/* ==============================================================================
        Deferred Shading
 ============================================================================== */
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
//uniform sampler2D gSpecular;
uniform sampler2D gLightSpacePosition;

/* ==============================================================================
        Directional / Spot Lights
 ============================================================================== */
uniform sampler2D depthMap;
uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D fluxMap;

float shadowFactor(vec4 lightSpacePosition, vec3 N, vec3 L){
	// perform perspective divide: clip space-> normalized device coords (done automatically for gl_Position)
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    // bring from [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 1.0;

    float currentDepth = projCoords.z;  

    float bias = baseBias * tan(acos(dot(N,L)));
    bias = clamp(bias, 0.0, 0.0005f);


    float shadow = 0.0;
    /** /
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
    /**/

    /**/
    vec2 texelSize = 1.0 / vec2(textureSize(depthMap, 0));
    for(int x = -1; x <= 1; x++){
    	for(int y = -1; y <= 1; y++){
    		float depth = texture(depthMap, projCoords.xy + vec2(x,y) * texelSize).r;
    		shadow += currentDepth - bias > depth ? 0.0 : 1.0;
    	}
    }
    shadow = shadow / 9.0;
    /**/

    return shadow;
}

/* ==============================================================================
        Stage Outputs
 ============================================================================== */

out vec4 outColor;

// dist, dist weight, cos(angle), angle weight
uniform vec4 indirectSampleParams;
uniform sampler2D lowResIndirect;

// assuming global value for shininess and specular
const float shininess = 16.0f;
const vec3 Specular = vec3(1.0f);

vec3 FragPos;
vec3 N;
vec3 Diffuse;
vec4 LightSpacePos;


 vec3 texture2D_bilinear(sampler2D t, vec2 uv, vec2 textureSize, vec2 texelSize)
{
    vec2 f = fract( uv * textureSize );
    uv += ( .5 - f ) * texelSize;    // move uv to texel centre
    vec3 tl = texture(t, uv).rgb;
    vec3 tr = texture(t, uv + vec2(texelSize.x, 0.0)).rgb;
    vec3 bl = texture(t, uv + vec2(0.0, texelSize.y)).rgb;
    vec3 br = texture(t, uv + vec2(texelSize.x, texelSize.y)).rgb;
    vec3 tA = mix( tl, tr, f.x );
    vec3 tB = mix( bl, br, f.x );
    return mix( tA, tB, f.y );
}

#ifdef DIRECTIONAL
vec3 directIllumination() {
	vec3 V = normalize(ViewPos - FragPos);
	vec3 L;

	float shadow = 1.0;
	vec3 retColor = vec3(0.0);

	L = normalize(-light.direction);
	shadow = shadowFactor(LightSpacePos, N, L);

	vec3 H = normalize(L + V);

	float NdotL = max(dot(N, L), 0.0);
	float NdotH = max(dot(N, H), 0.0);

	vec3 diff = vec3(0.0);
	vec3 spec = vec3(0.0);

	if (NdotL > 0.0){
		diff = light.emission * ( Diffuse * NdotL);
		spec = light.emission * ( Specular * pow(NdotH, shininess));
	}

	return ((diff + spec) * shadow);
}
#endif

#ifdef SPOTLIGHT
vec3 directIllumination() {
	vec3 V = normalize(ViewPos - FragPos);
	vec3 L;

	float shadow = 1.0;
	vec3 retColor = vec3(0.0);


	// Get Light Vector
	L = normalize(light.position - FragPos);
	shadow = shadowFactor(LightSpacePos, N, L);

	float theta = dot(L, normalize(-light.direction));
	if(theta <= light.cutoff){
		return vec3(0.0);
	}

	vec3 H = normalize(L + V);

	float NdotL = max(dot(N, L), 0.0);
	float NdotH = max(dot(N, H), 0.0);

	vec3 diff = vec3(0.0);
	vec3 spec = vec3(0.0);

	if (NdotL > 0.0){

		diff = light.emission * ( Diffuse * NdotL);
		spec = light.emission * ( Specular * pow(NdotH, shininess));

		float distance = length(light.position - FragPos);
		float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * pow(distance, 2.0));

		diff *= attenuation;
		spec *= attenuation;

	}

	return ((diff + spec) * shadow);
}
#endif

vec3 indirectIllumination() {
	vec3 result = vec3(0.0);
	vec3 indirect = vec3(0.0);
	// perform perspective divide: clip space-> normalized device coords (done automatically for gl_Position)

    vec3 projCoords = LightSpacePos.xyz / LightSpacePos.w;

    // bring from [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    for(int i=0; i < NUM_VPL; i++){
    	vec2 rnd = VPLSamples[i];
    	vec2 coords = vec2(projCoords.x + rsmRMax*rnd.x*sin(TWO_PI*rnd.y), projCoords.y + rsmRMax*rnd.x*cos(TWO_PI*rnd.y));

    	vec3 vplP = texture(positionMap, coords.xy).xyz;
    	vec3 vplN = normalize(texture(normalMap, coords.xy)).xyz;
    	vec3 vplFlux = texture(fluxMap, coords.xy).rgb;

        // long ver
        /** /
    	float dot1 = max(0.0, dot(vplN, normalize(FragPos - vplP)));
    	float dot2 = max(0.0, dot(N, normalize(vplP - FragPos)));
        indirect = vplFlux * (dot1 * dot2);
        /**/

        // original ver
        /**/
        float dot1 = max(0.0, dot(vplN, FragPos - vplP));
        float dot2 = max(0.0, dot(N, vplP - FragPos));
        float dist = length(vplP - FragPos);
        indirect = vplFlux * (dot1 * dot2) / (dist * dist * dist * dist);
        /**/

    	// frag == vpl pos???
    	//if(dist <= 0.0)
    	//	continue;

    	float weight = rnd.x * rnd.x;

    	indirect = indirect * weight;
    	//indirect = indirect * (1.0 / float(NUM_VPL));
    	result += indirect;
    }
    //result = result * (1.0 / float(NUM_VPL));
	return (result * Diffuse) * rsmIntensity;
}

void main(void) {

	FragPos       = texture(gPosition, texCoords).rgb;
	N             = texture(gNormal, texCoords).rgb;
	Diffuse       = texture(gDiffuse, texCoords).rgb;
	//Specular      = texture(gSpecular, texCoords).rgb;
	LightSpacePos = texture(gLightSpacePosition, texCoords);


	//outColor = vec4(directIllumination(), 1.0);

	//outColor = vec4(indirectIllumination(), 1.0);

	//outColor = vec4(directIllumination() + indirectIllumination(), 1.0);

	/** /
	if(isnan(outColor.x))
		outColor = vec4(1.0);

	if(isnan(outColor.y))
		outColor = vec4(1.0);

	if(isnan(outColor.z))
		outColor = vec4(1.0);
	/**/

	//outColor = vec4(directIllumination(pos, lightSpacePos, N, diffuse, specular), 1.0);
	//outColor = vec4(texture(lowResIndirect, texCoords).rgb, 1.0);

	/** /
	//vec3 direct = directIllumination(pos, lightSpacePos, N, diffuse, specular);

	vec2 texSize = textureSize(lowResIndirect, 0);
	vec2 texelSize = 1.0 / texSize;

	vec3 indirect = vec3(0.0);

	vec2 f = fract( texCoords * texSize );
    uv += ( .5 - f ) * texelSize;    // move uv to texel centre
    vec2 tl = texCoords;
    vec2 tr = texCoords + vec2(texelSize.x, 0.0);
    vec2 bl = texCoords + vec2(0.0, texelSize.y);
    vec2 br = texCoords + vec2(texelSize.x, texelSize.y);

    vec3 tA = mix( tl, tr, f.x );
    vec3 tB = mix( bl, br, f.x );

	outColor = vec4(direct + indirect, 1.0);

	/**/
	vec3 direct = directIllumination();

	vec2 texSize = textureSize(lowResIndirect, 0);
	vec2 texelSize = 1.0 / texSize;

	vec4 sampleX;
	vec4 sampleY;
	vec4 viable = vec4(0.0);
	int viableSamples = 0;

	// left up down right
	sampleX[0] = texCoords.x - texelSize.x ; sampleY[0] = texCoords.y;
	sampleX[1] = texCoords.x               ; sampleY[1] = texCoords.y + texelSize.y;
	sampleX[2] = texCoords.x               ; sampleY[2] = texCoords.y - texelSize.y;
	sampleX[3] = texCoords.x + texelSize.x ; sampleY[3] = texCoords.y;

	vec3 indirect = vec3(0.0);

	for(int i = 0; i < 4; i++){
		// use squared distance
		vec3 diff = FragPos - texture(gPosition, vec2(sampleX[i], sampleY[i])).rgb;
		if (dot(diff, diff) < indirectSampleParams.x){
			if(dot(N, texture(gNormal, vec2(sampleX[i], sampleY[i])).rgb) >= indirectSampleParams.z){
				viable[i] = 1.0;
				viableSamples += 1;
			}
		}
	}

	if(viableSamples >= 3){

		for(int i = 0; i < 4; i++){
			if(viable[i] == 1.0){
				indirect += texture(lowResIndirect, vec2(sampleX[i], sampleY[i])).rgb;
			}
		}

		// TEMPORARY "NORMALIZATION"
		indirect = indirect / float(viableSamples);
		outColor = vec4(direct + indirect, 1.0);
		//outColor = vec4(0.0, 1.0, 1.0, 1.0);

	}
	//if not, do raw indirect illum call
	else{
		outColor = vec4(direct + indirectIllumination(), 1.0);

		//tag non reconstructible
		//outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	/**/

	/**/
	/** /
	vec3 direct = directIllumination(pos, lightSpacePos, N, diffuse, specular);
	vec3 indirect = texture(lowResIndirect, texCoords).rgb;
	outColor = vec4(direct + indirect, 1.0);
	/**/
}