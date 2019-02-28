in vec3 position;
in vec3 color;

out vec4 out_color;

void main()
{
    out_color = vec4(color.x, color.y, color.z, 1.0);
} 