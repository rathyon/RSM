
in FragData {
	vec3 position;
	vec3 normal;
} vsIn;

uniform int mode;

layout(location = 0) out vec4 outColor;

void main(void) {


	vec3 gPosition = vsIn.position;
	vec3 gNormal = vsIn.normal;

	if (mode == 0){
		outColor = vec4(gPosition, 1.0);
	}
	else {
		outColor = vec4(gNormal, 1.0);
	}
}