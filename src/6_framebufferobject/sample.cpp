#include "sample.hpp"

#include <cstdio>
#include <cassert>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static void window_size_callback(GLFWwindow* window, int width, int height);

class Sample_Impl {
	friend void window_size_callback(GLFWwindow*, int, int);
public:
	Sample_Impl()
		: _GLFWwindow(nullptr), _windowWidth(640), _windowHeight(480)
	{
	}

	bool init()
	{
		if (!glfwInit())
				return false;

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		_GLFWwindow = glfwCreateWindow(_windowWidth, _windowHeight, 
				"Hello World", nullptr, nullptr);
		if (!_GLFWwindow)
		{
			glfwTerminate();
			return false;
		}

		glfwSetWindowSizeCallback(window(), window_size_callback);

		glfwMakeContextCurrent(_GLFWwindow);

		glewExperimental = true; // Needed for core profile
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			fprintf(stderr, "Error: %s\n", 
					glewGetErrorString(err));

			return false;
		}

		fprintf(stderr, "%s\n", glGetString(GL_VERSION));

		return true;
	}

	void destroy()
	{
		glfwTerminate();
	}

	GLFWwindow* window() { return _GLFWwindow; }
	int windowWidth() const { return _windowWidth; }
	int windowHeight() const { return _windowHeight; }

private:
	GLFWwindow* _GLFWwindow;
	int _windowWidth;
	int _windowHeight;
};

static Sample_Impl* impl = nullptr;

static void window_size_callback(GLFWwindow* window, int width, int height)
{
	impl->_windowWidth = width;
	impl->_windowHeight = height;
}


Sample::Sample()
{
	impl = new Sample_Impl();
}

Sample::~Sample()
{
	delete impl;
}

bool Sample::init()
{
	assert(impl);
	if (!impl->init())
		return false;

	if (!initContents())
		return false;

	return true;
}

void Sample::destroy()
{
	assert(impl);
	destroyContents();

	impl->destroy();
}

bool Sample::is_running() const
{
	assert(impl);
	return !glfwWindowShouldClose(impl->window());
}

void Sample::run()
{
	assert(impl);

	double lastTime = glfwGetTime();

	while (is_running()) {
		double currentTime = glfwGetTime();

		update(currentTime - lastTime);

		lastTime = currentTime;

		render();

		glfwSwapBuffers(impl->window());
		glfwPollEvents();
	}
}

int Sample::windowWidth() const
{
	assert(impl);

	return impl->windowWidth();
}

int Sample::windowHeight() const
{
	assert(impl);

	return impl->windowHeight();
}
