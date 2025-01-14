#version 400

uniform float time;

uniform float maxDist; // fog max distance
uniform float minDist; // fog min distance
uniform float fogDensity;
uniform vec3 fogColor;

uniform int randFreqX;
uniform int randFreqY;
uniform float randTexCoords;
uniform float randOffset;

uniform float randColor1R;
uniform float randColor1G;
uniform float randColor1B;
uniform float randColor2R;
uniform float randColor2G;
uniform float randColor2B;

uniform sampler2D texture1;
uniform sampler2D texture2;

in vec2 vUv;
in vec4 v_pos;
out vec4 FragColour;

void main() 
{	
	vec2 position = - 1.0 + 2.0 * vUv;

	vec4 noise = texture2D( texture1, vUv ); // noise texture
	vec2 T1 = vUv + vec2( 1.5, - 1.5 ) *  0.2 * time; // "time" 'animates' the texture
	vec2 T2 = vUv + vec2( - 0.5, 2.0 ) *  0.1 * time; // as above

	T1.x += noise.x * randFreqX;
	T1.y += noise.y * randFreqY;
	T2.x -= noise.y * randTexCoords; // this just offsets the texture coordinates
	T2.y += noise.z * randOffset; // but allows us to offset y&z in opposite directions

	float p = texture2D( texture1, T1).a; // get the alpha from the noise texture

	vec4 color = texture2D( texture2, T2); // coloured texture offset can here or above
				
	vec4 customColor = mix(vec4(vec3(randColor1R, randColor1G, randColor1B), 1.0), vec4(vec3(randColor2R, randColor2G, randColor2B), 1.0), p) * 3.0;
	vec4 temp = mix(color, customColor, 0.5);

	if( temp.r > 1.0 ) { temp.bg += clamp( temp.r - 2.0, 0.0, 100.0 ); }
	if( temp.g > 1.0 ) { temp.rb += temp.g - 1.0; }
	if( temp.b > 1.0 ) { temp.rg += temp.b - 1.0; }

	FragColour = temp;

	float dist = abs( v_pos.z ); // absolute value
	float fogFactor = (maxDist - dist)/(maxDist - minDist);
	fogFactor = clamp( fogFactor, 0.0, 1.0 ); // constrain range

	FragColour = mix( FragColour, vec4( fogColor, FragColour.w ), fogFactor );

}