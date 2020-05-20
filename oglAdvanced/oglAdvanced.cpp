#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <map>
#include <vector>

#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "Camera.h"
#include "App.h"
#include "Model.h"



using namespace std;
using namespace glm;



App app("pbr", 1280.0f, 720.0f);

int mainPBR() {
	Model sphere("models/sph2.obj");


	Shader shad("shaders/basic.shader");

	mat4 model(1.0f);
	model = scale(model, vec3(0.5f));

	mat4 view = lookAt(vec3(1, 1.5f, 1), vec3(0), vec3(0, 1, 0));
	mat4 proj = perspective(radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

	shad.setUniform4m("u_model", model);
	shad.setUniform4m("u_view", view);
	shad.setUniform4m("u_proj", proj);

	vec3 lightPos(1, 1, -1);
	shad.setUniformVec3("lightPos", lightPos);

	vec3 lightColor(1.0);
	shad.setUniformVec3("lightColor", lightColor);

	float rough = 0.1;
	float metallic = 0.5f;
	vec3 albedo(0.5f);

	shad.setUniform1f("rough", rough);
	while (app.isRunning()) {
		app.clear(vec3(0.3));

		ImGui::SliderFloat("roughness", &rough, 0.01f, 1.0f);
		ImGui::SliderFloat("metallic", &metallic, 0.01f, 1.0f);
		ImGui::SliderFloat3("lightPos", &lightPos[0], -6.0, 6.0);
		ImGui::SliderFloat3("albedo", &albedo[0],0.0, 1.0);
		ImGui::SliderFloat3("lightColor", &lightColor[0], 0.0, 1.0);


		shad.setUniformVec3("albedo", albedo);
		shad.setUniformVec3("lightColor", lightColor);
		shad.setUniformVec3("lightPos", lightPos);
		shad.setUniform1f("rough", rough);
		shad.setUniform1f("metallic", metallic);

		if (!app.refCamera->isEditing) {
			mat4 view = cam.GetViewMatrix();
			shad.setUniform4m("u_view", view);
		}
		
		vec3 viewPos = cam.Position;
		shad.setUniformVec3("viewPos", viewPos);
		sphere.draw(shad);

		app.swapBuffers();
	}


	app.terminate();
	return 0;
}




void printAllAttributes(unsigned handle) {
	GLint uniformsCount = 0;
	glGetProgramInterfaceiv(handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &uniformsCount);

	GLenum props[] = {
		GL_NAME_LENGTH,GL_TYPE,GL_LOCATION,GL_BLOCK_INDEX };

	cout << "active attributes" << endl;

	for (int i = 0; i < uniformsCount; i++) {
		GLint res[3];

		glGetProgramResourceiv(handle, GL_PROGRAM_INPUT, i, 3, props, 3, NULL, res);

		GLint nameBufSize = res[0] + 1;
		char * name = new char[nameBufSize];
		glGetProgramResourceName(handle, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);

		string type;
		if (res[1] == GL_FLOAT_VEC3) {
			type = "vec3";
		}
		if (res[1] == GL_FLOAT_VEC2) {
			type = "vec2";
		}
		cout << res[2] << "|" << name  << "|" << type << endl;


		delete[] name;
	}

}

void debugCallback(GLenum source, GLenum type,
	GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, const void *userParam) {
	cout << message << endl;
}

int main() {
	Model sphere("nanosuit/nanosuit.obj");

	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debugCallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

	Shader flat("shaders/flatShading.shader");
	Shader guro("shaders/guro.shader");
	Shader phong("shaders/phong.shader");
	Shader bFong("shaders/blinFong.shader");


	Shader* temp = &flat;

	mat4 model = scale(mat4(1.0f), vec3(0.2f));


	vec3 viewPos(1,0.5,1);

	mat4 view = lookAt(viewPos, vec3(0), vec3(0, 1, 0));
	mat4 proj = perspective(radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
	mat4 MVP = proj * view * model;
	

	mat3 normMat = mat3(transpose(inverse(view * model)));
	mat4 MV = view * model;
	


	vec3 lightPos = view * vec4(1, 2, -1,1);
	
	temp->setUniform4m("MV", MV);
	temp->setUniform4m("MVP", MVP);
	temp->setUniform3m("normMat", normMat);
	temp->setUniformVec3("lightPos", lightPos);
	vec3 tLightPos(1,2,-1);

	float shininess = 16;

	std::map<std::string, Shader*> shaderMap;
	shaderMap.insert({ "Flat",&flat });
	shaderMap.insert({ "Guro",&guro });
	shaderMap.insert({ "Phong",&phong });
	shaderMap.insert({ "Blin-Phong",&bFong });

	std::string currentItem = "Flat";
	std::vector<string> types{"Flat","Guro","Phong","Blin-Phong"};

	while (app.isRunning()) {
		app.clear();

		ImGui::SliderFloat("shininess", &shininess, 0, 128);
		ImGui::SliderFloat3("lightPos", &tLightPos[0],-10,10);

		if (ImGui::BeginCombo("##select shading type", currentItem.c_str(), ImGuiComboFlags_NoArrowButton))
		{
			for (int n = 0; n < types.size(); n++)
			{
				bool is_selected = (currentItem == types[n]);
				if (ImGui::Selectable(types[n].c_str(), is_selected))
					currentItem = types[n];
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		temp = shaderMap[currentItem];

		temp->setUniform1f("shininess", shininess);
		if (!cam.isEditing) {
			view = cam.GetViewMatrix();
		}
		
		vec3 pos = cam.Position;
		lightPos = view * vec4(tLightPos,1);
		MV = view * model;
		MVP = proj * view * model;
		normMat = mat3(transpose(inverse(view * model)));
		temp->setUniform4m("MV", MV);
		
		temp->setUniform4m("MVP", MVP);
		temp->setUniform3m("normMat", normMat);
		temp->setUniformVec3("lightPos", lightPos);

		sphere.draw(*temp);
	
		app.swapBuffers();
	}
	return 1;
}

int main2() {

	if (!glfwInit()) {
		throw exception("glfw: initialization error");
	}

	GLFWwindow * window = glfwCreateWindow(1280.0f, 720.0f, "uniform buffers test", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		throw exception("glfw: window creation error");
	}

	glfwMakeContextCurrent(window);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		throw exception("glad:  initialization error");
	}


	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debugCallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	Shader shad("shaders/circle.shader");
	printAllAttributes(shad.id);

	VAO va;
	VAO::Layout la;
	la.add<float>(2);
	la.add<float>(2);

	float vertices[] = {
		0.5f,0.5f,1.0f,0.0f,
		0.5f,-0.5f,1.0f,1.0f,
		-0.5,-0.5f,0.0f,1.0f,
		-0.5f,0.5f,0.0f,0.0f
	};

	unsigned int indices[] = {
		2,0,1,
		2,3,0
	};

	VBO vb(vertices, sizeof(vertices));
	IBO ib(indices, sizeof(indices));

	va.addLayout(la, vb, ib);

	mat4 proj = perspective(radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);
	shad.setUniform4m("proj", proj);
	//setting uniform block
	shad.bind();
	//find index of block
	unsigned blockIndex = glGetUniformBlockIndex(shad.id,"BlobSetting");

	//find size of block
	int blockSize;
	glGetActiveUniformBlockiv(shad.id, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

	// create Buffer
	GLubyte* blockBuffer = (GLubyte*)malloc(blockSize);

	const char* names[] = { "innerColor","outerColor","radiusInneer","radiusOuter" };

	//get indices of vars in uniform block
	unsigned indicesU[4];
	glGetUniformIndices(shad.id, 4, names, indicesU);

	//get offset of vars in uniform block
	int offset[4];
	glGetActiveUniformsiv(shad.id, 4, indicesU, GL_UNIFORM_OFFSET, offset);


	float outColor[]{ 0.0f,0.0f,0.0f,0.0f };
	float inColor[]{ 1, 1, 0.75, 1 };
	float inRadius = 0.25f;
	float outRadius = 0.45f;

	memcpy(blockBuffer + offset[0], &inColor[0], 4 * sizeof(float));
	memcpy(blockBuffer + offset[1], &outColor[0], 4 * sizeof(float));
	memcpy(blockBuffer + offset[2], &inRadius, sizeof(float));
	memcpy(blockBuffer + offset[3], &outRadius, sizeof(float));

	unsigned uniformBuffer;
	glGenBuffers(1,&uniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBuffer);


	while (!glfwWindowShouldClose(window)) {
		
		glClear(GL_COLOR_BUFFER_BIT);

		va.bind();
		shad.bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwPollEvents();
		glfwSwapBuffers(window);

	}

}



//lab
int mainPhong() {

	Shader fong("shaders/fongWorldSpace.shader");
	Shader lightShader("shaders/light.shader");
	Model container("models/container2.obj");
	Model light("models/sph2.obj");

	mat4 model = scale(mat4(1.0), vec3(0.3f));
	mat4 view = lookAt(vec3(1,2,1), vec3(0), vec3(0, 1, 0));
	mat4 proj = perspective(radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	fong.setUniform4m("u_model", model);
	fong.setUniform4m("u_view", view);
	fong.setUniform4m("u_proj", proj);


	vec3 lightPos(1, 1, -1);
	mat4 lightModel(1.0f);
	lightModel = translate(lightModel, lightPos);
	lightShader.setUniform4m("u_model", model);
	lightShader.setUniform4m("u_view", view);
	lightShader.setUniform4m("u_proj", proj);

	
	vec3 pos(1,2,1);

	vec3 ambientColor(0.2);
	vec3 diffColor(0.5);
	vec3 specColor(1.0f);

	vec3 ambIntensity(0.1);
	vec3 difIntensity(0.5);
	vec3 specIntensity(0.7);

	float shininess = 16;
	
	while (app.isRunning()) {
		app.clear();


		ImGui::Text("press e for editing");
		ImGui::Text("use wasd and mouse\n for camera movement");
		ImGui::Separator();
		
		


		ImGui::Text("Material properties");
		
		ImGui::SliderFloat3("ambient Color", &ambientColor[0], 0, 1);
		ImGui::SliderFloat3("diffuse Color", &diffColor[0], 0, 1);
		ImGui::SliderFloat3("specular Color", &specColor[0], 0, 1);
		ImGui::SliderFloat("shininess", &shininess, 0, 128);

		ImGui::Separator();
		ImGui::Text("Light properties");
		ImGui::SliderFloat3("ambient Intensity", &ambIntensity[0], 0, 1);
		ImGui::SliderFloat3("diffuse Intensity", &difIntensity[0], 0, 1);
		ImGui::SliderFloat3("specular Intensity", &specIntensity[0], 0, 1);
		ImGui::SliderFloat3("light position", &lightPos[0], -2, 2);


		fong.setUniform1f("shininess", shininess);

		fong.setUniformVec3("lightPos", lightPos);

		fong.setUniformVec3("ambientColor", ambientColor);
		fong.setUniformVec3("diffColor", diffColor);
		fong.setUniformVec3("specColor", specColor);

		fong.setUniformVec3("ambIntensity", ambIntensity);
		fong.setUniformVec3("difIntensity", difIntensity);
		fong.setUniformVec3("specIntensity", specIntensity);

		lightModel = translate(mat4(1.0f), lightPos);
		lightModel = scale(lightModel, vec3(0.1f));
		lightShader.setUniform4m("u_model", lightModel);
		lightShader.setUniform4m("u_view", view);

		if (!cam.isEditing) {
			pos = cam.Position;
			view = cam.GetViewMatrix();
			fong.setUniformVec3("viewPos", pos);
			fong.setUniform4m("u_view", view);

		}
		


		container.draw(fong);
		light.draw(lightShader);
		app.swapBuffers();
	}

	app.terminate();
}




int mainLab() {
	Model cone("modelsLab7/coneWireFrame.obj");
	Model cyl("modelsLab7/cylinder.obj");
	Model tor("modelsLab7/tor.obj");

	glEnable(GL_BLEND);
	glBlendFunc(GL_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	mat4 model = scale(mat4(1.0), vec3(0.3f));
	mat4 view = lookAt(vec3(1, 2, 1), vec3(0), vec3(0, 1, 0));
	mat4 proj = perspective(radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	mat4 model2 = scale(mat4(1.0), vec3(0.3f));
	model2 = translate(model2, vec3(2, 0, 2));

	Shader fogShader("shaders/fog.shader");
	vec3 pos;

	
	fogShader.setUniform4m("u_model", model);
	fogShader.setUniform4m("u_view", view);
	fogShader.setUniform4m("u_proj", proj);

	vec4 fogColor(0.5);
	vec4 renderColor(0.7);
	float fogMax = 9;
	float fogMin = 5;
	fogShader.setUniformVec4("renderColor", renderColor);
	fogShader.setUniform1f("fogMax", fogMax);
	fogShader.setUniform1f("fogMin", fogMin);
	vec4 halfAlpha(vec3(0.7), 0.5);
	fogShader.setUniformVec4("renderColor", halfAlpha);
	while (app.isRunning()) {
		app.clear(vec3(fogColor));

		ImGui::Text("Press e to edit variables");
		ImGui::Separator();
		ImGui::SliderFloat("fog max distance", &fogMax, 0, 100);
		ImGui::SliderFloat("fog min distance", &fogMin,0,100);
		ImGui::SliderFloat4("renderColor", &renderColor[0], 0, 1);
		ImGui::SliderFloat4("fogColor", &fogColor[0], 0, 1);
		fogShader.setUniformVec4("renderColor", renderColor);
		fogShader.setUniform1f("fogMax", fogMax);
		fogShader.setUniform1f("fogMin", fogMin);
		fogShader.setUniformVec4("fogColor", fogColor);
		if (!cam.isEditing) {
			pos = cam.Position;
			view = cam.GetViewMatrix();
			//fogShader.setUniformVec3("viewPos", pos);
			fogShader.setUniform4m("u_view", view);
		}
		
		float x = cos(glfwGetTime()) * 3;
		float y = sin(glfwGetTime()) * 3;
		mat4 modCyl = translate(mat4(1.0f), vec3(x, 0, y));
		modCyl = scale(modCyl, vec3(0.2));

		x = cos(glfwGetTime() + 20) * 2;
		y = sin(glfwGetTime() + 20) * 2;
		float z = cos(glfwGetTime());
		mat4 modTor = translate(mat4(1.0f), vec3(x, z, y));
		modTor = scale(modTor, vec3(0.2));
		
		fogShader.setUniformVec4("renderColor", renderColor);
		fogShader.setUniform4m("u_model", modCyl);
		cyl.draw(fogShader);

		fogShader.setUniform4m("u_model", model);
		cone.draw(fogShader);


		fogShader.setUniformVec4("renderColor", halfAlpha);
		fogShader.setUniform4m("u_model", modTor);
		tor.draw(fogShader);

		

		app.swapBuffers();
	}


	app.terminate();
}