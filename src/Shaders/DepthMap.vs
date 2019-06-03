
layout(location = 0) in vec3 Position;

uniform mat4 ModelMatrix;
uniform mat4 lightSpaceMatrix;	

void main()
{
	// Return position in Light MVP coordinates
    gl_Position = lightSpaceMatrix * ModelMatrix * vec4(Position, 1.0);
}