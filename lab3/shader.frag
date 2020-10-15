#version 330 core

in vec2 TexCoords;
in vec3 Normal;

out vec4 Color;

uniform vec3 lightDir;
uniform sampler2D text;

void main (){
	vec3 result = (vec3(texture(text, TexCoords)) + vec3(0.5)) *
	max(dot(normalize(-lightDir), normalize(Normal)), 0.0);
	Color = vec4(result, 1.0);
}