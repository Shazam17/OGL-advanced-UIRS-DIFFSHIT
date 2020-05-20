#shader vertex
#version 330 core


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tCord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec2 v_tCord;
out vec3 fragCord;
out vec3 normal;


void main() {

	gl_Position = u_proj * u_view * u_model * vec4(pos, 1);
	v_tCord = tCord;
	fragCord = (u_model * vec4(pos, 1)).xyz;
	normal = mat3(transpose(inverse(u_model))) * norm;
}





#shader fragment
#version 330 core
out vec4 color;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
};


in vec2 v_tCord;
in vec3 fragCord;
in vec3 normal;

uniform vec3 viewPos;
uniform Material material;


//float zNear = 0.1;
//float zFar = 100.0;
//LINEAR Z BUFFER
//float LinearizeDepth(float depth)
//{
	// преобразуем обратно в NDC
	//float z = depth * 2.0 - 1.0;
	//return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
//}
//color = vec4(0.5, 0.5, 0.5, 1);

//float depth = LinearizeDepth(gl_FragCoord.z) / zFar;
//color = vec4(vec3(depth), 1);
//VISUALIZE Z BUFFER

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
};

float gamma = 2.2;

uniform Light light;
void main() {

	float distance = length(light.position - fragCord);

	float attenuation = 1.0 / (distance);

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragCord);

	vec3 tColor = texture(material.texture_diffuse1, v_tCord).xyz;

	tColor = pow(tColor, vec3(gamma));

	vec3 specColor = texture(material.texture_specular1, v_tCord).xyz;

	float diff = max(dot(norm, lightDir), 0.0f);

	
	vec3 viewDir = normalize(viewPos - fragCord);

	//FONG
	vec3 reflected = reflect(-lightDir, norm);
	//float spec = pow(max(dot(reflected, viewDir),0.0f),32);

	//BLIN FONG
	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm,halfWayDir),0.0f), 32);

	vec3 ambient = light.ambient * tColor * attenuation;
	vec3 diffuse = light.diffuse * diff * tColor * attenuation;
	vec3 specular = light.specular * spec * specColor * attenuation;

	vec3 result = ambient + diffuse + specular;
	result = pow(result, vec3(1.0 / gamma));
	color = vec4(result, 1);
}