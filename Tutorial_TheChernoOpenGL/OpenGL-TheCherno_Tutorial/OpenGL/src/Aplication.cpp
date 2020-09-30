#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


struct ShaderProgramSource 
{
	std::string vertexSource;
	std::string fragmentSource;
};

static ShaderProgramSource  ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType 
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) // MIENTRAS SEA VALIDO OBTENER UNA LINEA ENTRARA AL BUCLE.
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
			ss[(int)type] << line << '\n';
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);


	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) 
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile "<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
			<< std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);
		std::cin.get();
		return -1;
	}

	return id;
}

static unsigned int CreatedShader(const std::string& vertexShader, const std::string& fragmentShader) 
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error al inicializar glew" << std::endl;
		std::cin.get();
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[]
	{
		-0.5f, -0.5f, //index 0
		 0.5f, -0.5f, //index 1
		 0.5f,  0.5f, //index 2
		-0.5f,  0.5f, //index 3
	};
	
	const int countPositions = 8;
	const int stride = 2; // cantidad 
	const int offset = sizeof(float) * 2; // 
	const int countVetices = 4;

	unsigned int indexBuffer[] = 
	{
		0, 1, 2,
		2, 3, 0
	};
	const int countIndexBuffer = 6;

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, countPositions * sizeof(float),positions , GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, stride, GL_FLOAT, GL_FALSE, offset, 0);

	unsigned int indexBufferObject;
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, countIndexBuffer * sizeof(unsigned int), indexBuffer, GL_STATIC_DRAW);

	ShaderProgramSource source = ParseShader("res/shaders/BasicShader.shader");

	//std::cout << "VERTEX" << std::endl;
	//std::cout << source.vertexSource << std::endl;
	//std::cout << "FRAGMENT" << std::endl;
	//std::cout << source.fragmentSource << std::endl;

	unsigned int shader = CreatedShader(source.vertexSource, source.fragmentSource);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, countIndexBuffer, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	//glDeleteShader(shader);
	glfwTerminate();
	return 0;
}
//DIBUJAMOS UN TRIANGULO
//glBegin(GL_TRIANGLES);
//glVertex2d(-0.5f, -0.5f);
//glVertex2d(0, 0.5f);
//glVertex2d(0.5f, -0.5f);
//glEnd();
//----------------------//