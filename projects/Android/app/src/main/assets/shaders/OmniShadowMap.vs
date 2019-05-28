
layout(location = 0) in vec3 Position;

uniform mat4 ModelMatrix;

void main()
{
	// Return position in Light MVP coordinates
    gl_Position = ModelMatrix * vec4(Position, 1.0);
}