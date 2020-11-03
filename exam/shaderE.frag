#version 330 core

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_emissive1;
uniform Light light;
uniform float shininess;
uniform vec3 matSpec;

void main(){
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));

	vec3 normal = vec3(texture(texture_normal1, TexCoords));
	normal = normal * 2.0 - 1.0;
	normal = normalize(normal);

	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = light.specular * spec * matSpec;

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
	                    light.quadratic * distance * distance);

	vec3 emissive = vec3(texture(texture_emissive1, TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	FragColor = vec4(ambient + diffuse + specular + emissive, 1.0);
	// FragColor = vec4(diffuse, 1.0);
}