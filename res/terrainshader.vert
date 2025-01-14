#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec3 Pos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = aTexCoords;
	Normal = normalize(model * vec4(aNormal, 0)).xyz;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	Pos = (model * vec4(aPos, 1)).xyz;
}