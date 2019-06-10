
in FragData {
	vec3 position;
	vec3 normal;
} vsIn;

uniform int mode;

layout(location = 0) out vec4 outColor;

void main(void) {

	if (mode == 0){
		outColor = vec4(vsIn.position, 1.0);
	}
	else {
		outColor = vec4(vsIn.normal, 1.0);
	}
}