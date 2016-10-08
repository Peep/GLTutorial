#define GLEW_STATIC

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <assert.h>

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

	// Triangle definition
	float triangle[] =
	{
		// Positions	 // Colors
		0.5, -0.5, 0.0,  1.0, 0.0, 0.0, // Bottom right
		-0.5, -0.5, 0.0,  0.0, 1.0, 0.0, // Bottom left
		0.0,  0.5, 0.0,  0.0, 0.0, 1.0  // Top
	};

	// Source of a GLSL vertex shader
	const char* vertexShaderSource = R"(
		#version 330 core
		layout(location = 0) in vec3 position; // The position variable has attribute position 0
		layout(location = 1) in vec3 color;	// The color variable has attribute position 1

		out vec3 ourColor; // Output a color to the fragment shader
	
		void main()
		{
			gl_Position = vec4(position, 1.0); // vec3 to vec4's contructor
			ourColor = color; // Set ourColor to the input color we got from the vertex data
		}
	)";

	// Source of a GLSL fragment shader
	const char* fragmentShaderSource = R"(
		#version 330 core
		in vec3 ourColor;
		out vec4 color;

		void main()
		{
			color = vec4(ourColor, 1.0f);
		} 
	)";

	// Setup shader object
	int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Compile shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Setup GLSL fragment shader
	int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Create shader program to link both shaders
	int shaderProgram;
	shaderProgram = glCreateProgram();

	// Attach shaders to the program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Delete shader objects after compilation/linkage
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Make VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	// Bind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Copy a triangle to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	// Tell GL how to interpret the vertex data

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// Begin render loop
	while (!glfwWindowShouldClose(window))
	{
		// Check and call events
		glfwPollEvents();

		// Activate the shader
		glUseProgram(shaderProgram);

		// Rendering commands
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2) + 0.5;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		// Draw
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Swap
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}