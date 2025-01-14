#version 400

out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D diffuse;

 
void main()
{
	vec4 color;
	color = texture2D(diffuse, TexCoords);
    FragColor = color;
}