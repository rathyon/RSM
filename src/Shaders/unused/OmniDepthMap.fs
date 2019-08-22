
in vec3 fragPos;

uniform vec3 lightPos;
uniform float far;

void main(void) {
	/**/
	// get distance between fragment and light source
    float dist = length(fragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by far
    dist = dist / far;
    
    // write this as modified depth
    gl_FragDepth = dist;
    /**/
}