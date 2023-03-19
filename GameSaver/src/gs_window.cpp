#include "gs_window.h"

// std
#include <stdexcept>


namespace md
{
	GSWindow::GSWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name}
	{
		initWindow();
	}

	GSWindow::~GSWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void GSWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // tell glfw not to create opengl context
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);	// enable window resize after creation


		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
	}

	void GSWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}

	void GSWindow::framebufferResizedCallback(GLFWwindow* window, int width, int height)
	{
		auto gsWindow = reinterpret_cast<GSWindow*>(glfwGetWindowUserPointer(window));
		gsWindow->framebufferResized = true;
		gsWindow->width = width;
		gsWindow->height = height;
	}


}// namespace md