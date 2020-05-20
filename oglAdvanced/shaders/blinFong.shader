#shader vertex
#version 330 core


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tCord;


uniform mat4 M;
uniform mat4 MV;
uniform mat4 MVP;

uniform mat3 normMat;

out vec3 fragPos;
out vec3 normal;
out vec2 texCord;

void main() {

	fragPos = (MV * vec4(pos, 1)).xyz;
	normal = normMat * norm;
	texCord = tCord;
	gl_Position = MVP * vec4(pos, 1);
}


#shader fragment
#version 330 core


out vec4 color;

in vec3 fragPos;
in vec3 normal;
in vec2 texCord;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform float shininess;

vec3 ambientColor = vec3(0.1);
vec3 specularColor = vec3(0.7);
void main() {

	vec3 tColor = texture(texture_diffuse1, texCord).xyz;

	vec3 viewDir = normalize(-fragPos);
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 norm = normalize(normal);

	float diff = max(dot(lightDir,norm), 0.0);

	
	float spec = 0.0;
	if (diff > 0) {
		vec3 halfWay = normalize(viewDir + lightDir);
		spec = pow(max(dot(norm, halfWay), 0.0), shininess);
	}
	
	vec3 result = ambientColor * tColor + diff * tColor + specularColor * spec ;

	color = vec4(result, 1);
}