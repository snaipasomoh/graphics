#version 330 core

in vec3 Colors;
in vec3 Normal;

out vec4 Color;

uniform vec3 lightDir;

void main (){
	vec3 result = Colors * max(dot(normalize(-lightDir), normalize(Normal)), 0.0);
	Color = vec4(result, 1.0);
}