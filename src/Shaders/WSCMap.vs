
layout(location = 0) in vec3 Position;

uniform mat4 ModelMatrix;
uniform mat4 lightSpaceMatrix;

out FragData {
	vec3 position; // world space
} vsOut;

void main()
{
	vsOut.position = vec3(ModelMatrix * vec4(Position, 1.0));
	// Return position in Light MVP coordinates
    gl_Position = lightSpaceMatrix * ModelMatrix * vec4(Position, 1.0);
}