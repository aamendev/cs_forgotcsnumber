#ifndef GLFW_WINDOW_CALLBACKS_H
#define GLFW_WINDOW_CALLBACKS_H
#include "lengine/types.h"
#include "glfw_vulkan_window.h"
namespace lina { namespace renderer { namespace backend { namespace callbacks{
    void key(GLFWwindow* win, i32 key, i32 scancode, i32 action, i32 mods);
    void mouse_button(GLFWwindow* win, i32 key, i32 action, i32 mods);
    void mouse_move(GLFWwindow* win, f64 x, f64 y);
    void window_resize(GLFWwindow* win, i32 w, i32 h);
    void window_close(GLFWwindow* window);
}}}}
#endif
