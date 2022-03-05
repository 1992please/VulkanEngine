#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <string>

namespace ve
{ 
    class VeWindow
    {
        public:
            VeWindow(int w, int h, std::string name);
            ~VeWindow();
            bool shouldClose() { return glfwWindowShouldClose(window); }

            // Remove copy constructor
            VeWindow(const VeWindow&) = delete;
            VeWindow& operator=(const VeWindow&) = delete;

        private:
            GLFWwindow* window;
            void initWindow();
            const int width;
            const int height;
            std::string windowName;
    };
}