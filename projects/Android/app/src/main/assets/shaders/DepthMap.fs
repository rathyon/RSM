
/** /
in FragData {
	vec3 position;
	vec3 normal;
} vsIn;
/**/

//layout(location = 0) out vec3 gPosition;
//layout(location = 1) out vec3 gNormal;

void main(void) {
	//gPosition = vsIn.position;
	//gNormal = vsIn.normal;

	//gPosition = vec3(1.0, 0.0, 0.0);
	//gNormal = vec3(0.0, 1.0, 0.0);

	// not sure if I can just comment this line
	//gl_FragDepth = gl_FragCoord.z;
}