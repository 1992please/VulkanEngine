#include "first_app.h"

namespace ve
{
    void FirstApp::run()
    {
        while(!veWindow.shouldClose())
        {
            glfwPollEvents();
        }
    }
}