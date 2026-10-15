#include "glfw_window_callbacks.h"
#include <GLFW/glfw3.h>
namespace lina { namespace renderer { namespace backend { namespace callbacks{
    using wsettings = typename lina::renderer::backend::window<GraphicsBackend::Vulkan>::settings;
    void key(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        wsettings& p = 
            *reinterpret_cast<wsettings*>(glfwGetWindowUserPointer(window));
        switch (action)
        {
            case GLFW_PRESS:
                {
                    auto* event = new events::key_press((input::key_code)key, false,
                            (mods & GLFW_MOD_SHIFT), mods & GLFW_MOD_CAPS_LOCK);
                    p.events.push_back(event);
                    break;
                }
            case GLFW_REPEAT:
                {
                    auto* event = new events::key_press((input::key_code)key, true,
                            (mods & GLFW_MOD_SHIFT), mods & GLFW_MOD_CAPS_LOCK);
                    p.events.push_back(event);
                    break;
                }
            case GLFW_RELEASE:
                {
                    auto* event = new events::key_release((input::key_code)key,
                            (mods & GLFW_MOD_SHIFT), mods & GLFW_MOD_CAPS_LOCK);
                    p.events.push_back(event);
                    break;
                }
        }
    }

    void mouse_button(GLFWwindow* window, int button, int action, int mod)
    {
        wsettings& p = 
            *reinterpret_cast<wsettings*>(glfwGetWindowUserPointer(window));
        switch(action)
        {
            case GLFW_PRESS:
                {
                    auto* event = new events::mouse_button_pressed((input::mouse_code)button);
                    p.events.push_back(event);
                    break;
                }
            case GLFW_RELEASE:
                {
                    auto* event = new events::mouse_button_released((input::mouse_code)button);
                    p.events.push_back(event);
                    break;
                }
        }
    }

    void mouse_move(GLFWwindow* window, f64 xpos, f64 ypos)
    {
        wsettings& p = 
            *reinterpret_cast<wsettings*>(glfwGetWindowUserPointer(window));
       auto* event = new events::mouse_move(xpos, ypos);
       p.events.push_back(event);
    }
    void window_resize(GLFWwindow* window, int width, int height)
    {
       auto p_window = reinterpret_cast<wsettings*>
           (glfwGetWindowUserPointer(window));
       auto* event = new events::window_resize(width, height);
       p_window->events.push_back(event);
       p_window->width = width;
       p_window->height = height;
       p_window->resized = true;
    }
    void window_close(GLFWwindow* window)
    {
       auto p_window = reinterpret_cast<wsettings*>
           (glfwGetWindowUserPointer(window));
       auto* event = new events::window_close();
       p_window->events.push_back(event);
    }
}}}}
