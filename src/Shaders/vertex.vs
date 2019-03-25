
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;

uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;

uniform cameraBlock {
	mat4 ViewMatrix;
	mat4 ProjMatrix;
	mat4 ViewProjMatrix;
	vec3 ViewPos;
};

out FragData {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
} vsOut;

void main()
{
	vsOut.position  = vec3(ModelMatrix * vec4(Position, 1.0));
	vsOut.normal    = normalize(NormalMatrix * Normal);
	vsOut.texCoords = TexCoords;

    gl_Position = ViewProjMatrix * ModelMatrix * vec4(Position ,1.0);
}