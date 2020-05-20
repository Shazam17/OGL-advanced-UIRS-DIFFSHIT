#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

#include <string>
#include <iostream>
#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;
using namespace glm;


Camera cam;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = 0;
float lastY = 0;
bool firstMouse = true;


void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		if (!cam.isEditing) {
			cam.isEditing = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			//glfwSetCursorPos(window, 640, 360);
		}
	}
	
	if (cam.isEditing) {
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, deltaTime);

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (cam.isEditing) {
		return;
	}
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	cam.ProcessMouseMovement(xoffset, yoffset);
}

class App {
private:
	GLFWwindow* window;



	float width;
	float height;
	float aspect;

	
public:
	Camera* refCamera;
	App(string name, float p_width = 640.0f, float p_height = 480.0f) : width(p_width), height(p_height) {
		if (!glfwInit()) {
			throw exception("glfw: initialization error");
		}
		
		window = glfwCreateWindow(p_width, p_height, name.c_str(), NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			throw exception("glfw: window creation error");
		}

		glfwMakeContextCurrent(window);

		cam = Camera(glm::vec3(3.0f, 3.0f, 3.0f),vec3(0,1,0),-130,-36);
		refCamera = &cam;
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize OpenGL context" << std::endl;
			throw exception("glad:  initialization error");
		}

		glfwSetCursorPosCallback(window, mouse_callback);

		// tell GLFW to capture our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

		glEnable(GL_DEPTH_TEST);
		aspect = width / height;

		//Setup IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

	}

	vec3 lastForward;
	double xPos, yPos;


	bool isRunning() {
		return !glfwWindowShouldClose(window);
	}

	bool pressed = false;
	void clear(vec3 color = vec3(0.0f)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(color.r, color.g, color.b, 1.0f);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (ImGui::Button("exit editing")) {
			pressed = true;
			
			
		}
	}

	void swapBuffers() {

		//Render
		if (pressed) {
			glfwSetCursorPos(window, lastX, lastY);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cam.isEditing = false;
			pressed = false;
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	void terminate() {
		//Shutdown
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();
		glfwTerminate();
	}
};



//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void processInput(GLFWwindow *window);

/*
App app("BLINN FONG",1200.0f,800.0f);
int main(void)
{
	bool editMode = false;


	Shader basic("shaders/basic.shader");

	Model model("nanosuit/nanosuit.obj");

	mat4 proj = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	mat4 view = cam.GetViewMatrix();
	mat4 mod = scale(mat4(1.0f), vec3(0.2f));
	//mod = rotate(mod, radians(-90.0f), vec3(1.0f, 0.0, 0.0f));
	basic.setUniform4m("u_model", mod);
	basic.setUniform4m("u_view", view);
	basic.setUniform4m("u_proj", proj);

	vec3 lightPos(5.0f);
	vec3 amb(0.2f);
	vec3 diff(0.5f);
	vec3 spec(1.0f);

	basic.setUniformVec3("light.ambient", amb);
	basic.setUniformVec3("light.diffuse", diff);
	basic.setUniformVec3("light.specular", spec);
	basic.setUniformVec3("light.position", lightPos);



	while (app.isRunning()) {
		app.clear();

		ImGui::InputFloat3("lightPos", &lightPos[0],4);
		basic.setUniformVec3("light.position", lightPos);

		if (ImGui::Button("exit editing")) {
			editMode = false;
			app.changeCursor();
		}

		mat4 view = cam.GetViewMatrix();
		basic.setUniform4m("u_view", view);
		vec3 pos = cam.Position;
		basic.setUniformVec3("viewPos", pos);
		model.draw(basic);

		app.swapBuffers();
	}

	app.terminate();
	return 0;
}



















*/