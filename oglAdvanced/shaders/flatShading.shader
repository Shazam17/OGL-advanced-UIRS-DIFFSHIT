#shader vertex
#version 330 core


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;


uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 M;
uniform mat3 normMat;

uniform vec3 lightPos;
uniform float shininess;
flat out vec3 result;
void main() {

	vec3 norm = normalize(normMat * normal);
	vec3 fragPos = (MV * vec4(pos, 1)).xyz;

	gl_Position = MVP * vec4(pos, 1);


	vec3 lightDir = normalize(lightPos - fragPos);

	float diffuse = max(dot(lightDir, norm), 0.0);



	vec3 reflected = reflect(-lightDir, norm);
	vec3 viewDir = normalize(-fragPos);
	float spec = 0.0;
	if (diffuse > 0.0) {
		spec = pow(max(dot(reflected, viewDir), 0.0), shininess);
	}


	result = vec3(0.1) + vec3(0.2) * diffuse + vec3(0.2) * spec;
}




#shader fragment
#version 330 core


struct Light {
	vec3 lightPos;

	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

struct Material {
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;

	float shininess;
};


uniform Light light;
uniform Material material;
uniform vec3 lightPos;
uniform float shininess;

out vec4 color;

flat in vec3 result;

void main() {




	color = vec4(result, 1);
}
