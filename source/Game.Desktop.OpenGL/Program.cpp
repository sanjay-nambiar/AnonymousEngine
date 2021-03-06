#include "Pch.h"
#include "Program.h"
#include <fstream>
#include "ShaderCompiler.h"

namespace AnonymousEngine
{
	Program::Program(int width, int height, std::string title)
		: width(width), height(height), title(title), window(nullptr), shaderProgram(0), VAO(0)
	{
		// Using code from AnonymousEngine.Desktop
		list = new SList<int>();
	}

	Program::~Program()
	{
		delete list;
	}

	void Program::Init()
	{
		// Initalize window parameters
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		// create window
		window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		if (window == nullptr)
		{
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}
		glfwMakeContextCurrent(window);
			
		// initialize glad
		if (!gladLoadGL())
		{
			throw std::runtime_error("Failed to initialize GLEW");
		}

		// initalize viewport
		glfwGetFramebufferSize(window, nullptr, nullptr);
		glViewport(0, 0, width, height);

		// register keyboard handler
		glfwSetKeyCallback(window, KeyCallback);
		InitShaders();
	}

	void Program::GameLoop()
	{
		// game loop exiting on window close or ESC keypress
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			Draw();
			glfwSwapBuffers(window);
		}
	}

	void Program::ShutDown()
	{
		glfwTerminate();
	}

	void Program::InitShaders()
	{
		// compile shaders
		GLuint vertexShader = ShaderCompiler::CompileShaderFromFile("shaders\\default.vert", ShaderCompiler::ShaderType::VERTEX_SHADER);
		GLuint fragmentShader = ShaderCompiler::CompileShaderFromFile("shaders\\default.frag", ShaderCompiler::ShaderType::FRAGMENT_SHADER);
		std::vector<GLuint> shaders = {vertexShader, fragmentShader};
		shaderProgram = ShaderCompiler::CreateProgramWithShaders(shaders);
		glUseProgram(shaderProgram);

		// free shaders since program is created
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// vertices and indices to vertices for a tringle
		GLfloat vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};
		GLuint indices[] = {
			0, 1, 2
		};

		// create VertexBufferObject, ElementBufferObject and VertexArrayObject
		GLuint VBO;
		glGenBuffers(1, &VBO);
		GLuint EBO;
		glGenBuffers(1, &EBO);
		glGenVertexArrays(1, &VAO);

		// configure the size and stride attributes of vertex buffer object and element buffer object
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(nullptr));
		// bind the VAO for triangle and set to use our shader program
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
		glUseProgram(shaderProgram);
	}

	void Program::Draw()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	void Program::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		UNREFERENCED_PARAMETER(scancode);
		// if ESC key is pressed and there are no modifier keys pressed, ask to close the output window
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && mode == 0)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}
