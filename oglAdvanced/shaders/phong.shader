#shader vertex
#version 330 core


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tCord;


uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 M;
uniform mat3 normMat;

uniform vec3 lightPos;

out vec3 fragPos;
out vec3 norm;
out vec2 texCord;

void main() {

	norm = normalize(normMat * normal);
	fragPos = (MV * vec4(pos, 1)).xyz;

	gl_Position = MVP * vec4(pos, 1);
	texCord = tCord;
}




#shader fragment
#version 330 core


uniform vec3 lightPos;
uniform float shininess;

out vec4 color;

in vec3 fragPos;
in vec3 norm;
in vec2 texCord;

uniform sampler2D texture_diffuse1;

void main() {

	vec3 tColor = texture(texture_diffuse1, texCord).xyz;

	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 normal = normalize(norm);


	float diffuse = max(dot(lightDir, normal), 0.0);



	vec3 reflected = reflect(-lightDir, normal);
	vec3 viewDir = normalize(-fragPos);
	float spec = 0.0;
	if (diffuse > 0.0) {
		spec = pow(max(dot(reflected, viewDir), 0.0), shininess);
	}


	vec3 result = vec3(0.1)  * tColor + tColor * diffuse + vec3(0.2) * spec;


	color = vec4(result, 1);
}
