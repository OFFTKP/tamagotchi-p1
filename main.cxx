#include "tama.hxx"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "frontend.hxx"

int screen_width = 640, screen_height = 320;

int main(int argc, const char** argv) {
    if (!glfwInit())
		return 1;
    const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow *window = glfwCreateWindow(screen_width, screen_height, "Dear ImGui", NULL, NULL);
    glfwMakeContextCurrent(window);
	bool err = glewInit() != GLEW_OK;
	if (err)
		return 1;
	glViewport(0, 0, screen_width, screen_height);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
    Frontend frontend;
    frontend.Load(argv[1]);
    while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		frontend.Draw();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glViewport(0, 0, screen_width, screen_height    );
		glfwSwapBuffers(window);
	}
    return 0;
}