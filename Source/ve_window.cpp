#include "ve_window.h"


#include <stdexcept>

namespace ve
{
    VeWindow::VeWindow(int w, int h, std::string name) : 
        width(w), 
        height(h), 
        windowName(name),
        frameBufferResized(false)
    {
        initWindow();
    }

    VeWindow::~VeWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

	void VeWindow::frameBufferResizedCallback(GLFWwindow* window, int width, int height)
	{
        VeWindow* veWindow = reinterpret_cast<VeWindow*>(glfwGetWindowUserPointer(window));
        veWindow->frameBufferResized = true;
        veWindow->width = width;
        veWindow->height = height;
	}

	void VeWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferResizedCallback);
    }

    void VeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
            throw std::runtime_error("failed to create window surface");
    }
}