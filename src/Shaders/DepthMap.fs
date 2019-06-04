
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;

in FragData {
	vec3 position;
	vec3 normal;
} vsIn;

void main(void) {
	gPosition = vsIn.position;
	gNormal = vsIn.normal;

	// not sure if I can just comment this line
	//gl_FragDepth = gl_FragCoord.z;
}