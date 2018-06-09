#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream> 

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
	/*							CREATING TRIANGLES							*/
	/************************************************************************/

	// First triangle:
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	/************************************************************************/
	/*							VERTEX BUFFER OBJECT						*/
	/************************************************************************/

	// Generate vertex buffer object VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// Bind VBO to ARRAY_BUFFER => as VBO is a vertex we use ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Copy data to buffer memory (buffer_used, size of array, array, type of draw (static/dynamic)
	// Static = modified once and used as source many times, Dynamic = modified repeatedly and used many times
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


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

		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// Clean all resources
	glfwTerminate();
	return 0;
}