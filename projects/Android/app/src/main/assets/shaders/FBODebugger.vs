
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoords;

out vec2 outTexCoords;

void main()
{
    outTexCoords = TexCoords;
    gl_Position = vec4(Position, 1.0f);
}