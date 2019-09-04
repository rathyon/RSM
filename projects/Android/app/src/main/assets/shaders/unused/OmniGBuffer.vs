
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;

uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;

uniform int face;
uniform mat4 lightSpaceMatrices[6];

// Passes everything in world coordinates to the fragment shader
out FragData {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
} vsOut;

void main()
{
	// These in world coordinates
	vsOut.position  = vec3(ModelMatrix * vec4(Position, 1.0));
	vsOut.normal    = normalize(NormalMatrix * Normal);
	vsOut.texCoords = TexCoords;
	
	// Return position in Light MVP coordinates
    gl_Position = lightSpaceMatrices[face] * vec4(vsOut.position, 1.0);
}