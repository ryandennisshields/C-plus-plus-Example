#version 400

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 transform;

out VS_OUT
{ vec2 texCoords;
}vs_out;

out vec3 v_norm;
out vec4 v_pos; 

void main()
{
	v_norm = aNormal;
	v_pos = vec4(aPos, 1.0);
	vs_out.texCoords = aTexCoords;
	gl_Position = transform * vec4(aPos, 1.0);
}