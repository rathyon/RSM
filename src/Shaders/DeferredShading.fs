
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
uniform float VPLWeights[NUM_VPL];
uniform vec2 VPLSamples[NUM_VPL];
uniform vec2 VPLCoords[NUM_VPL];
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

vec3 projCoords;

float shadowFactor(vec3 lightSpacePosition, vec3 N, vec3 L){
    if(projCoords.z > 1.0)
        return 1.0;

	/**/
    float closestDepth = texture(depthMap, projCoords.xy).r;
    float currentDepth = projCoords.z; 

    float bias = baseBias * tan(acos(dot(N,L)));
    bias = clamp(bias, 0.0, 0.0005f);

    float shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
    /**/

    /** /
    float currentDepth = projCoords.z; 

    float bias = baseBias * tan(acos(dot(N,L)));
    bias = clamp(bias, 0.0, 0.0005f);


    float shadow = 0.0;
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

// values manually imported per scene...
uniform float shininess;
uniform vec3 Specular;

vec3 FragPos;
vec3 N;
vec3 Diffuse;
vec3 LightSpacePos;

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

    //vec3 projCoords = LightSpacePos.xyz / LightSpacePos.w;

    // bring from [-1,1] to [0,1]
    //projCoords = projCoords * 0.5 + 0.5;

    for(int i=0; i < NUM_VPL; i++){
    	vec2 rnd = VPLSamples[i];
    	//vec2 coords = vec2(projCoords.x + rsmRMax*rnd.x*sin(TWO_PI*rnd.y), projCoords.y + rsmRMax*rnd.x*cos(TWO_PI*rnd.y));
        vec2 coords = vec2(projCoords.x + VPLCoords[i].x, projCoords.y + VPLCoords[i].y);

    	vec3 vplP = texture(positionMap, coords.xy).xyz;
    	//vec3 vplN = texture(normalMap, coords.xy).xyz * 2.0 - 1.0;
    	vec3 vplN = texture(normalMap, coords.xy).xyz;

        float dot2 = max(0.0, dot(N, vplP - FragPos));

        vec3 vplFlux = texture(fluxMap, coords.xy).rgb;

        float dist = length(vplP - FragPos);
        float dot1 = max(0.0, dot(vplN, FragPos - vplP));

    	// frag == vpl pos???
    	if(dist <= 0.0)
    		continue;

        indirect = (vplFlux * (dot1 * dot2) / (dist * dist * dist * dist)) * VPLWeights[i];

    	//indirect = indirect * VPLWeights[i];
    	//indirect = indirect * (1.0 / float(NUM_VPL));
    	result += indirect;
    }
    //result = result * (1.0 / float(NUM_VPL));
	return (result * Diffuse) * rsmIntensity;
}

uniform vec2 lowResIndirectSize;
uniform vec2 texelSize;

void main(void) {

	FragPos       = texture(gPosition, texCoords).rgb;
	//N             = texture(gNormal, texCoords).rgb * 2.0 - 1.0;
	N             = texture(gNormal, texCoords).rgb;
	Diffuse       = texture(gDiffuse, texCoords).rgb;
	//Specular      = texture(gSpecular, texCoords).rgb;
	LightSpacePos = texture(gLightSpacePosition, texCoords).rgb;
	projCoords = LightSpacePos;


	// Testing
	//outColor = vec4(directIllumination(), 1.0);
	//outColor = vec4(indirectIllumination(), 1.0);
	//outColor = vec4(texture(lowResIndirect, texCoords).rgb, 1.0);

	//outColor = vec4(texture(fluxMap, texCoords).rgb, 1.0);


	// Deferred Naive
	outColor = vec4(directIllumination() + indirectIllumination(), 1.0);

	// Deferred Interpolated
	/** /
	vec3 direct = directIllumination();

	//ivec2 texSizei = textureSize(lowResIndirect, 0);
	//vec2 texSize = vec2(float(texSizei.x), float(texSizei.y));
	vec2 texSize = lowResIndirectSize;
	//vec2 texelSize = 1.0 / texSize;

	vec2 uv = texCoords;
	vec2 f = fract( uv * texSize );
    uv += ( .5 - f ) * texelSize;    // move uv to texel centre

    vec2 tl_uv = uv;
	vec2 tr_uv = uv + vec2(texelSize.x, 0.0);
	vec2 bl_uv = uv + vec2(0.0, texelSize.y);
	vec2 br_uv = uv + vec2(texelSize.x, texelSize.y);

    vec3 tl = texture(lowResIndirect, tl_uv).rgb;
    vec3 tr = texture(lowResIndirect, tr_uv).rgb;
    vec3 bl = texture(lowResIndirect, bl_uv).rgb;
    vec3 br = texture(lowResIndirect, br_uv).rgb;

	int viableSamples = 0;

	vec3 indirect = vec3(0.0);

	// Bilinear Screen Space Interpolation
	// top left
	vec3 diff = FragPos - texture(gPosition, tl_uv).rgb;
	if ((dot(diff, diff) < indirectSampleParams.x) && (dot(N, texture(gNormal, tl_uv).rgb) >= indirectSampleParams.z)){
		viableSamples += 1;
	}
	else{
		tl = tr;
	}
	// top right
	diff = FragPos - texture(gPosition, tr_uv).rgb;
	if ((dot(diff, diff) < indirectSampleParams.x) && (dot(N, texture(gNormal, tr_uv).rgb) >= indirectSampleParams.z)){
		viableSamples += 1;
	}
	else{
		tr = tl;
	}
	// bottom left
	diff = FragPos - texture(gPosition, bl_uv).rgb;
	if ((dot(diff, diff) < indirectSampleParams.x) && (dot(N, texture(gNormal, bl_uv).rgb) >= indirectSampleParams.z)){
		viableSamples += 1;
	}
	else{
		bl = br;
	}
	// bottom right
	diff = FragPos - texture(gPosition, br_uv).rgb;
	if ((dot(diff, diff) < indirectSampleParams.x) && (dot(N, texture(gNormal, br_uv).rgb) >= indirectSampleParams.z)){
		viableSamples += 1;
	}
	else{
		br = bl;
	}

	// basically its always bilinear interpolation with 4, I just do a cheat when the 4th one is invalid...
	if(viableSamples >= 3){
	    vec3 tA = mix( tl, tr, f.x );
	    vec3 tB = mix( bl, br, f.x );
		outColor = vec4(direct + mix( tA, tB, f.y ), 1.0);
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

	// Home made "Average" interpolation
    /** /
		vec3 diff = FragPos - texture(gPosition, tl_uv).rgb;
	if ((dot(diff, diff) < indirectSampleParams.x) && (dot(N, texture(gNormal, tl_uv).rgb) >= indirectSampleParams.z)){
		viableSamples += 1;
		indirect += tl;
	}
	// top right
	diff = FragPos - texture(gPosition, tr_uv).rgb;
	if ((dot(diff, diff) < indirectSampleParams.x) && (dot(N, texture(gNormal, tr_uv).rgb) >= indirectSampleParams.z)){
		viableSamples += 1;
		indirect += tr;
	}
	// bottom left
	diff = FragPos - texture(gPosition, bl_uv).rgb;
	if ((dot(diff, diff) < indirectSampleParams.x) && (dot(N, texture(gNormal, bl_uv).rgb) >= indirectSampleParams.z)){
		viableSamples += 1;
		indirect += bl;
	}
	// bottom right
	diff = FragPos - texture(gPosition, br_uv).rgb;
	if ((dot(diff, diff) < indirectSampleParams.x) && (dot(N, texture(gNormal, br_uv).rgb) >= indirectSampleParams.z)){
		viableSamples += 1;
		indirect += br;
	}

	if(viableSamples >= 3) {
		indirect = indirect / viableSamples;
		outColor = vec4(direct + indirect, 1.0f);
	}
	else{
		outColor = vec4(direct + indirectIllumination(), 1.0);

		//tag non reconstructible
		//outColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
	}
    /**/