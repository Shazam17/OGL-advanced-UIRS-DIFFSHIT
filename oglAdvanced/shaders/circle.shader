#shader vertex
#version 430 core


layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tCord;


uniform mat4 proj;

out vec2 texCord;

void main() {

	texCord = tCord;
	gl_Position = proj * vec4(pos,-2, 1);

}



#shader fragment
#version 430 core



out vec4 color;
in vec2 texCord;

layout(binding = 0) uniform BlobSetting{
	vec4 innerColor;
	vec4 outerColor;
	float radiusInneer;
	float radiusOuter;
};


void main() {
	vec2 dV = texCord - 0.5;

	float dist = sqrt(dV.x*dV.x + dV.y*dV.y);
	color = mix(innerColor, outerColor, smoothstep(radiusInneer, radiusOuter, dist));
}
