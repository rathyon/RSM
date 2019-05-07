
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;
layout(location = 3) in vec3 Tangent;

uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;

uniform cameraBlock {
	mat4 ViewMatrix;
	mat4 ProjMatrix;
	mat4 ViewProjMatrix;
	vec3 ViewPos;
};

// Passes everything in world coordinates to the fragment shader
out FragData {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
	vec3 tangent;
} vsOut;

void main()
{
	// Everything in world coordinates
	vsOut.position  = vec3(ModelMatrix * vec4(Position, 1.0));
	vsOut.normal    = normalize(NormalMatrix * Normal);
	vsOut.texCoords = TexCoords;

	// Return position in MVP coordinates
    gl_Position = ViewProjMatrix * vec4(vsOut.position, 1.0);
}