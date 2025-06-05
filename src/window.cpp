#include <window.hpp>

#include <stdexcept>

namespace zsl
{
namespace window
{
    window::window()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        m_window = glfwCreateWindow(1280, 720, WINDOW_NAME.c_str(), NULL, NULL);
        if (!m_window)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window\n");
        }
        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(0);
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    window::~window()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void update(window& window_)
    {
        glfwPollEvents();
        if (is_pressed(window_, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window_.m_window, true);
        }
    }

    void swap_buffers(window& window_)
    {
        glfwSwapBuffers(window_.m_window);
    }

    bool is_focused(const window& window_)
    {
        return glfwGetWindowAttrib(window_.m_window, GLFW_FOCUSED);
    }

    bool should_close(const window& window_)
    {
        return glfwWindowShouldClose(window_.m_window);
    }

    window_size get_size(const window& window_)
    {
        int width;
        int height;
        glfwGetWindowSize(window_.m_window, &width, &height);
        return glm::ivec2(width, height);
    }

    void set_scroll_callback(window& window_, GLFWscrollfun callback)
    {
        glfwSetScrollCallback(window_.m_window, callback);
    }

    void set_cursor_state(window& window_, bool show)
    {
        glfwSetInputMode(window_.m_window, GLFW_CURSOR, (show) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    void set_cursor_pos(window& window_, cursor_pos cursor_pos_)
    {
        glfwSetCursorPos(window_.m_window, cursor_pos_.x, cursor_pos_.y);
    }

    cursor_pos get_cursor_pos(window& window_)
    {
        double xpos{0};
        double ypos{0};
        glfwGetCursorPos(window_.m_window, &xpos, &ypos);
        return glm::vec2(xpos, ypos);
    }

    bool is_pressed(const window& window_, u32 glfw_key)
    {
        return glfwGetKey(window_.m_window, glfw_key) == GLFW_PRESS;
    }
}
}