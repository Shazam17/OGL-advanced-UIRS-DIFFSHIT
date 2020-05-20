#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;


uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 fragPos;
out vec3 normal;



void main() {

	fragPos = (u_model * vec4(pos, 1)).xyz;
	gl_Position = u_proj * u_view * u_model * vec4(pos, 1);

	normal = mat3(transpose(inverse(u_model))) * norm;
}


#shader fragment
#version 330 core

uniform vec3 lightPos;
uniform vec3 viewPos;


uniform vec3 ambientColor;
uniform vec3 diffColor;
uniform vec3 specColor;

uniform vec3 ambIntensity;
uniform vec3 difIntensity;
uniform vec3 specIntensity;

uniform float shininess;

out vec4 color;

in vec3 fragPos;
in vec3 normal;


void main() {

	vec3 ambient = ambientColor * ambIntensity;
	
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 viewDir = normalize(viewPos - fragPos);


	float diff = max(dot(lightDir, norm), 0.0);
	
	vec3 diffuse = diffColor * difIntensity;

	float spec = 0.0;

	if (diff > 0.0) {
		vec3 reflected = reflect(-lightDir, norm);

		spec = pow(max(dot(reflected,viewDir),0.0), shininess);
	}

	vec3 specular = specColor * specIntensity * spec;

	vec3 result = ambient + diffuse + specular;

	color = vec4(result, 1);

}


