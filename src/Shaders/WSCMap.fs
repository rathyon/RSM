
in FragData {
	vec3 position; // world space
} vsIn;

out vec4 outColor;

void main(void) {
	outColor = vec4(vsIn.position, 1.0);
}