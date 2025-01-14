#version 330 core

in vec3 Pos;
in vec2 TexCoords;
in vec3 Normal;

out vec4 fragColor;

uniform sampler2D texture;
uniform vec3 lightPos;

uniform mat4 model;
uniform mat4 projection;

void main() 
{	
	
	// ambient
	vec3 ambientColor = vec3(1, 1, 1);
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * ambientColor;
	
	// diffuse
	vec3 diffuseColor = vec3(1, 1, 1);
	vec3 lightDirection = normalize(lightPos - Pos);
	float brightness = max(dot(Normal, lightDirection), 0.0);
	vec3 diffuse = diffuseColor * brightness;
	
	vec3 result = (ambient + diffuse) * texture2D(texture, TexCoords).xyz;

	fragColor = vec4(result, 1.0f);
}