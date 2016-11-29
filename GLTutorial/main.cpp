#define GLEW_STATIC

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <assert.h>
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

void multiMatrix(float* matrix, float* vector, float* out)
{
	float transMatrix[4] = {
		vector[0], vector[1], vector[2], 1
	};

	out[0] = (matrix[0] * transMatrix[0]) + (matrix[1] * transMatrix[1]) + (matrix[2] * transMatrix[2]) + (matrix[3] * transMatrix[3]);
	out[1] = (matrix[4] * transMatrix[0]) + (matrix[5] * transMatrix[1]) + (matrix[6] * transMatrix[2]) + (matrix[7] * transMatrix[3]);
	out[2] = (matrix[8] * transMatrix[0]) + (matrix[9] * transMatrix[1]) + (matrix[10] * transMatrix[2]) + (matrix[11] * transMatrix[3]);
	out[3] = (matrix[12] * transMatrix[0]) + (matrix[13] * transMatrix[1]) + (matrix[14] * transMatrix[2]) + (matrix[15] * transMatrix[3]);
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

	float identityMatrix[] =
	{
		1, 0, 0, 0,
		0, 1, 0, 0.5,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	// TODO: Transfer each myButt result into the positions.

	float myButt[] = { 0, 0, 0, 0 };

	multiMatrix(identityMatrix, vertices, myButt);
	multiMatrix(identityMatrix, (vertices + 6), myButt);
	multiMatrix(identityMatrix, (vertices + 12), myButt);

	// Texture coordinates
	float texCoords[] = {
		0.0f, 0.0f, // Bottom left corner
		1.0f, 0.0f, // Lower right corner
		0.5f, 1.0f, // Top center corner
	};

	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	float borderColor[] = { 1.0f, 1.0f, 0.f, 1.0f };
	glTextureParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, pixelSize;
	unsigned char* image = stbi_load("container.jpg", &width, &height, &pixelSize, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);
	glBindTexture(GL_TEXTURE_2D, 0);

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

	int fps = 0;

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

		float timeValue = (float)glfwGetTime();

		float greenValue = (float)((sin(timeValue) / 2) + 0.5);
		GLint ourColorLocation = glGetUniformLocation(ourShader.Program, "ourColor");
		glUniform3f(ourColorLocation, 0.0f, greenValue, 0.0f);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);

		fps++;
	}

	// Properly de-allocate all resources once they've outlived their purposes
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// Terminate GLFW, clearing any resources allocated by GLFW
	glfwTerminate();
	return 0;
}