#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;


uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;


out vec3 fragPos;

void main() {

	gl_Position = u_proj * u_view * u_model * vec4(pos, 1);
	fragPos = (u_view * u_model * vec4(pos, 1)).xyz;
}


#shader fragment
#version 330 core

out vec4 color;

in vec3 fragPos;
uniform vec3 viewPos;
uniform vec4 renderColor;
uniform float fogMax;
uniform float fogMin;
uniform vec4 fogColor;


void main() {

	float fogDistance = (fogMax - abs(fragPos.z)) / (fogMax - fogMin);
	float factor = clamp(fogDistance, 0.0, 1.0);
	color = mix(fogColor, renderColor, factor);
	
	

}


