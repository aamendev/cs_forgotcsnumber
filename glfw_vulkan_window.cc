#include "lengine/lrenderer/vulkan_backend/window.h"
#include "glfw_window_callbacks.h"
#include "glfw_vulkan_window.h"
namespace lina { namespace renderer { namespace backend { 
    glfw_vulkan_window::~glfw_vulkan_window()
    {
        if (mwindow)
        {
            glfwDestroyWindow(mwindow);
        }
        glfwTerminate();
    }
    b8 glfw_vulkan_window::create(settings& s)
    {
        msettings = s;
        if (!init())
        {
            return false;
            glfwTerminate();
        }
        return true;
    }

    b8 glfw_vulkan_window::init()
    {
        if (!glfwInit())
        {
            return false;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // pure vulkan
        glfwWindowHint(GLFW_RESIZABLE, msettings.resizable);
        glfwGetError(nullptr);
        mwindow = glfwCreateWindow(
                msettings.width,
                msettings.height, msettings.name.c_str(),
                NULL, NULL);
        if (!mwindow) return false;
        glfwSwapInterval(4);
        glfwSetInputMode(mwindow, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
        register_callbacks();
        return true;
    }
    void glfw_vulkan_window::register_callbacks()
    {
        glfwSetWindowUserPointer(mwindow, &msettings);
        glfwSetWindowSizeCallback(mwindow, callbacks::window_resize);
        glfwSetKeyCallback(mwindow, callbacks::key);
        glfwSetCursorPosCallback(mwindow, callbacks::mouse_move);
        glfwSetMouseButtonCallback(mwindow, callbacks::mouse_button);
        glfwSetWindowCloseCallback(mwindow, callbacks::window_close);
    }
    void glfw_vulkan_window::update()
    {
        glfwPollEvents();
        glfwGetFramebufferSize(mwindow, (i32*)&msettings.width,  (i32*)&msettings.height);
        glfwSwapBuffers(mwindow);
    }
}}}

