
out vec4 FragColor;
in vec2 outTexCoords;
  
uniform sampler2D FBO;
  
void main()
{
    FragColor = vec4(texture(FBO, outTexCoords).r);
} 