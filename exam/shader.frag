#version 330 core

struct Light {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform Light light;

void main(){
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));

	vec3 normal = vec3(texture(texture_normal1, TexCoords));
	normal = normal * 2.0 - 1.0;
	normal = normalize(TBN * normal);

	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));

	float shininess = 32;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = light.specular * spec * vec3(1.0);

	FragColor = vec4(ambient + diffuse + specular, 1.0);
	// FragColor = vec4(diffuse, 1.0);
}