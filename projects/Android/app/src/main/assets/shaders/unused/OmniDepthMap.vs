
layout(location = 0) in vec3 Position;

uniform mat4 ModelMatrix;

uniform int face;
uniform mat4 lightSpaceMatrices[6];

out vec3 fragPos;

void main()
{
	fragPos = vec3(ModelMatrix * vec4(Position, 1.0));

	// Return position in Light MVP coordinates
    gl_Position =  lightSpaceMatrices[face] * ModelMatrix * vec4(Position, 1.0);
}