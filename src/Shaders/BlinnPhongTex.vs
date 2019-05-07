
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

struct Light {
	vec3 position;
	vec3 direction;
	vec3 emission;
	float intensity;
	float linear;
	float quadratic;
	int type;
	bool state;
	float cutoff;
};

uniform Light lights[NUM_LIGHTS];

// Passes everything in tangent space coordinates to the fragment shader
out FragData {
	vec3 position;
	vec2 texCoords;
	vec3 lightPos[NUM_LIGHTS];
	vec3 lightDir[NUM_LIGHTS];
	vec3 viewPos;
} vsOut;

void main()
{
	vec3 T = normalize(NormalMatrix * Tangent);
	vec3 N = normalize(NormalMatrix * Normal);
	vec3 B = cross(N, T);

	vec3 position = vec3(ModelMatrix * vec4(Position, 1.0));

	// orthogonal matrices' transpose = their inverse!
	mat3 TBN = transpose(mat3(T,B,N));

	// everything in tangent space coordinates
	vsOut.position = TBN * position;
	for(int i=0; i < NUM_LIGHTS; i++){
		vsOut.lightPos[i] = TBN * lights[i].position;
		vsOut.lightDir[i] = TBN * lights[i].direction;
	}
	vsOut.viewPos = TBN * ViewPos;

	vsOut.texCoords = TexCoords;
	// Return position in MVP coordinates
    gl_Position = ViewProjMatrix * vec4(position, 1.0);
}