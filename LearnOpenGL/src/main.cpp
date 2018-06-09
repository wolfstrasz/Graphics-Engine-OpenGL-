#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>

// Basic Vertex Shader string
const char *basicVertexShaderSource = "#version 450 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// Basic Fragment Shader string
const char *fragmentShaderSource = "#version 450 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

// For debugging
int  success;							// success flag for debugging
char infoLog[512];						// log msg for debugging

// resize window and viewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Check if btn is pressed while window is on, if ESC btn is pressed window should close
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main(void)
{

	/************************************************************************/
	/*							SETUP GLFW									*/
	/************************************************************************/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);						///> Use Versions 4.0 ++
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);						///> Use version 4.5
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);		///> Set profile to Core profile
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			    ///> Set profile to Forward profile (macOS)


	/************************************************************************/
	/*							CREATING WINDOW								*/
	/************************************************************************/
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// set Context to window
	glfwMakeContextCurrent(window);
	
	/************************************************************************/
	/*							LOAD OpenGL FUNCTIONS						*/
	/************************************************************************/
	
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));
	
	// Set up View Port
	glViewport(0, 0, 800, 600);

	// Set up FrameBuffer Size Callback for given window
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/************************************************************************/
	/*						SHADERS: SET && COMPILE							*/
	/************************************************************************/

	// Vertex Shader: set and compile
	unsigned int basicVertexShader;
	basicVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(basicVertexShader, 1, &basicVertexShaderSource, NULL);
	glCompileShader(basicVertexShader);

	// check if compilation of Vertex Shaderwas successful
	glGetShaderiv(basicVertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(basicVertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment Shader: set and compile
	unsigned int basicFragmentShader;
	basicFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(basicFragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(basicFragmentShader);

	// check if compilation of Fragment Shader was successful
	glGetShaderiv(basicFragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(basicFragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Creating Shader program
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	// Adding shaders to program
	glAttachShader(shaderProgram, basicVertexShader);
	glAttachShader(shaderProgram, basicFragmentShader);

	// Linking shaders
	glLinkProgram(shaderProgram);

	// check if compilation of the shader program was successful
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout<< "ERROR::PROGRAM::LINK::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// set shader program to be used
	glUseProgram(shaderProgram);

	// delete shaders to free space;
	glDeleteShader(basicVertexShader);
	glDeleteShader(basicFragmentShader);


	/************************************************************************/
	/*							CREATING TRIANGLES							*/
	/************************************************************************/

	// Define vertices:
	float vertices[] = {
		0.5f,  0.5f, 0.0f,   // [0] = top right 
		0.5f, -0.5f, 0.0f,   // [1] = bottom right
		-0.5f, -0.5f, 0.0f,  // [2] = bottom left
		-0.5f,  0.5f, 0.0f   // [3] = top left 
	};
	unsigned int indices[] = { 
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	/************************************************************************/
	/*							BINDING VERTEX OBJECTS						*/
	/************************************************************************/

	unsigned int VAO, VBO, EBO;
	// Generate Vertex Array Object (VAO)
	glGenVertexArrays(1, &VAO);
	// Generate Vertex Buffer Object (VBO)
	glGenBuffers(1, &VBO);
	// Generate Element_Array Buffer Object (EBO)
	//glGenBuffers(1, &EBO);

	// Fitst Bind VAO ! 
	glBindVertexArray(VAO);

	// Bind VBO then copy data to buffer memory 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind EBO then copy data to buffer memory
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Link Vertex Attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr); //*// check for GL_TRUE
	glEnableVertexAttribArray(0);


	/************************************************************************/
	/*							RENDER LOOP									*/
	/************************************************************************/

	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);

		// rendering commands:
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// Draw a triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// Clean all resources
	glfwTerminate();
	return 0;
}

/************************************************************************/
/*							UNNAMED										*/
/************************************************************************/