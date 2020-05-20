#shader vertex
#version 330


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tCord;

layout(location = 3) in vec2 tang;
layout(location = 4) in vec2 bitTang;



uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec3 norm;
out vec3 fragCord;

void main() {

	gl_Position = u_proj * u_view * u_model * vec4(pos, 1);
	fragCord = (u_model * vec4(pos, 1)).xyz;
	norm = mat3(transpose(inverse(u_model))) * normal;
}






#shader fragment
#version 330

out vec4 o_color;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float rough;
uniform float metallic;

const float PI = 3.14159265359;

in vec3 norm;
in vec3 fragCord;
float DistributionGGX(vec3 N, vec3 H, float a)
{
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float k)
{
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx1 = GeometrySchlickGGX(NdotV, k);
	float ggx2 = GeometrySchlickGGX(NdotL, k);

	return ggx1 * ggx2;
}

vec3 F0 = vec3(0.04);
//vec3 F0 = vec3(1.0f,0.71f,0.29);

//cosTheta (normal,viewDir)
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

uniform vec3 lightColor;
uniform vec3 albedo;
void main() {

	
	
	F0 = mix(F0, albedo, metallic);
	
	float distance = length(fragCord - lightPos);
	float attenuation = 1.0 / (distance*distance);
	vec3 radiance = attenuation * lightColor;

	vec3 L = normalize(lightPos - fragCord);
	vec3 V = normalize(viewPos - fragCord);
	vec3 N = normalize(norm);

	vec3 H = normalize(L + V);

	float NDF = DistributionGGX(N,H,rough);
	float G = GeometrySmith(N, V, L, rough);
	vec3 F = fresnelSchlick(max(dot(H,     V), 0.0), F0);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;

	vec3 specular = numerator / denominator;


	//specular component
	vec3 kS = F;

	// diffuse component
	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - metallic;

	//normal dot lightDir
	float NdotL = max(dot(N, L), 0.0);


	vec3 Lo = (kD * albedo / PI + specular) * NdotL * radiance;


	vec3 ambient = vec3(0.03) * albedo ;
	vec3 color = ambient + Lo;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));
	o_color = vec4(color, 1);
}