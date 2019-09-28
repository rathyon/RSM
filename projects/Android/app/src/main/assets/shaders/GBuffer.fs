
in FragData {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
	vec4 lightSpacePosition;
} vsIn;

//Material parameters
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

uniform sampler2D diffuseTex;

vec3 fetchDiffuse(){
	if (diffuse.r < 0.0){
		return texture(diffuseTex, vsIn.texCoords).rgb;
	}
	else{
		return diffuse;
	}
}

/** /
layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gDiffuse;
layout(location = 3) out vec4 gLightSpacePosition;
/**/

/**/
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gDiffuse;
layout(location = 3) out vec3 gLightSpacePosition;
/**/

void main(void) {
	/** /
	gPosition = vec4(vsIn.position, 1.0);
	gNormal = vec4(vsIn.normal, 1.0) * 0.5 + 0.5;
	//gNormal = vec4(vsIn.normal, 1.0);
	gDiffuse = vec4(fetchDiffuse(), 1.0);
	gLightSpacePosition = vsIn.lightSpacePosition;
	/**/
	/**/
	gPosition = vsIn.position;
	gNormal = vsIn.normal * 0.5 + 0.5;
	gDiffuse = fetchDiffuse();
	gLightSpacePosition = ( vsIn.lightSpacePosition.xyz / vsIn.lightSpacePosition.w ) * 0.5 + 0.5;
	/**/
}