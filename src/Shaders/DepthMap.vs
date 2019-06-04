
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;

uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 lightSpaceMatrix;

// Passes everything in world coordinates to the fragment shader
out FragData {
	vec3 position;
	vec3 normal;
} vsOut;

void main()
{
	// These in world coordinates
	vsOut.position  = vec3(ModelMatrix * vec4(Position, 1.0));
	vsOut.normal    = normalize(NormalMatrix * Normal);
	
	// Return position in Light MVP coordinates
    gl_Position = lightSpaceMatrix * ModelMatrix * vec4(Position, 1.0);
}