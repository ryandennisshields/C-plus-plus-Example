#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float bloomIntensity;

void main()
{
    vec3 hdrColor = texture(scene, TexCoords).rgb; // get original pixel color
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb; // get bloom pixel color
    hdrColor += bloomColor * bloomIntensity; // additive blending of original pixel color and bloom color + bloom intensity factor
    FragColor = vec4(hdrColor, 1.0);
}