
in FragData {
	vec3 position;
	vec3 normal;
} vsIn;

// depth = 0
// position = 1
// normal = 2
// flux = 3

uniform int mode;

layout(location = 0) out vec4 outColor;

void main(void) {

	if (mode == 0){
		outColor = vec4(vec3(gl_FragDepth), 1.0);
	}
	if (mode == 1){
		outColor = vec4(vsIn.position, 1.0);
	}
	if (mode == 2){
		outColor = vec4(vsIn.normal, 1.0);
	}
	else{
		outColor = vec4(1.0, 1.0, 0.0, 1.0);
	}
}