#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>

static unsigned int CompileShader(unsigned int type , const std::string& source)
{
	// 替换着色器对象中的源代码
	unsigned int id = glCreateShader(type);  
	const char* src = source.c_str(); // &source[0]
	glShaderSource(id, 1, &src, nullptr);	//如果传入长度为null，那么每个字符串都被认定为以null结尾
	glCompileShader(id);

	// Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));			// 栈上动态分配内存 也可以在堆上new完delete
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
			<< " shader !" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();  // 创建一个空的程序对象并返回一个可以引用它的非零值。
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
	window = glfwCreateWindow(640, 480, "m1gL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// 首先需要创建一个有效的openGL context
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error!" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};

	// 定义缓冲区
	unsigned int buffer;
	glGenBuffers(1, &buffer);    // 生成一个缓冲区 返回buffer id
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	// STREAM 修改一次最多使用几次  STATIC 修改一次使用多次  DYNAMIC 多次修改多次使用
	// STATIC适用于不经常变化(静态物体)，STREAM则适用于缓冲区经常被更新的(粒子)。
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);				//创建并初始化缓冲区对象的数据存储

	glEnableVertexAttribArray(0);
	// 自动使用当前绑定的buffer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	// Shader
	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;"
		"\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(position);\n"
		"}\n";

	std::string fragmentShader =
		"#version 330 core\n"
		"\n"
		"out vec4 color;"
		"\n"
		"void main()\n"
		"{\n"
		"   color = vec4(0.5,0.0,0.0,1.0);\n"
		"}\n";

	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

	}
	//清理着色器
	glDeleteShader(shader);

	glfwTerminate();
	return 0;
}