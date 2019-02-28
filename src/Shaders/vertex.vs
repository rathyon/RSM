in vec3 pos;
in vec3 rgb;

out vec3 color;

void main()
{
	color = vec3(rgb.x, rgb.y, rgb.z);
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}