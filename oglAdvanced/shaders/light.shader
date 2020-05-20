#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;


uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;




void main() {

	gl_Position = u_proj * u_view * u_model * vec4(pos, 1);

}


#shader fragment
#version 330 core

out vec4 color;

void main() {

	

	color = vec4(1,1,1, 1);

}


