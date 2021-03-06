#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace ve
{
    class VeWindow
    {
    public:
        VeWindow(int w, int h, std::string name);
        ~VeWindow();

		// Remove copy constructor
		VeWindow(const VeWindow&) = delete;
		VeWindow& operator=(const VeWindow&) = delete;

        bool shouldClose() { return glfwWindowShouldClose(window); }
        VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
        bool wasWindowResized() { return frameBufferResized; }
        void resetWindowResizedFlag() { frameBufferResized = false; }
        GLFWwindow* getGLFWwindow() const { return window; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    private:
        static void frameBufferResizedCallback(GLFWwindow* window, int width, int height);
        void initWindow();

        int width;
        int height;
        bool frameBufferResized;

        std::string windowName;
        GLFWwindow *window;
    };
}