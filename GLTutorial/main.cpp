#define GLEW_STATIC

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <assert.h>
#include "Shader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Close the application on escape.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

GLFWwindow* MakeWindow()
{
	// Make window
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	// Make viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	return window;
}

int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	auto window = MakeWindow();

	// Init GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Add event listener to the window
	glfwSetKeyCallback(window, key_callback);

	// Build and compile our shader program
	Shader ourShader("shader.vs", "shader.frag");

	// Triangle definition
	float vertices[] =
	{
		// Positions	 // Colors
		0.5, -0.5, 0.0,  1.0, 0.0, 0.0, // Bottom right
		-0.5, -0.5, 0.0,  0.0, 1.0, 0.0, // Bottom left
		0.0,  0.5, 0.0,  0.0, 0.0, 1.0  // Top
	};

	// Make VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// Bind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Copy a triangle to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Tell GL how to interpret the vertex data

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO

	// Begin render loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the triangle
		ourShader.Use();

		GLfloat timeValue = glfwGetTime();

		GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
		GLint ourColorLocation = glGetUniformLocation(ourShader.Program, "ourColor");
		glUniform3f(ourColorLocation, 0.0f, greenValue, 0.0f);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Properly de-allocate all resources once they've outlived their purposes
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// Terminate GLFW, clearing any resources allocated by GLFW
	glfwTerminate();
	return 0;
}