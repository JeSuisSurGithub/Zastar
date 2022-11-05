#include <yzcontrol.hpp>

float g_fov = 75.f;

namespace yz
{
    controls_::controls_(GLFWwindow* window)
    :
    m_camera_xyz(glm::vec3(0.f, 0.f, 5.f)),
    m_move_speed(.25f),
    m_view_speed(.00025f),
    m_horizontal_angle(3.14f),
    m_vertical_angle(0.f),
    m_show_mouse(false),
    m_show_mouse_cooldown(0),
    m_show_mouse_cooldown_max(500) // 500ms
    {
        glfwSetScrollCallback(window, scroll_callback);
    }

    controls_::~controls_() {}

    glm::mat4 process_input(controls& context, GLFWwindow* window, float delta_time)
    {
        int width{0};
        int height{0};
        glfwGetWindowSize(window, &width, &height);

        if (!context.m_show_mouse)
        {
            double xpos{0};
            double ypos{0};
            glfwGetCursorPos(window, &xpos, &ypos);
            glfwSetCursorPos(window, static_cast<double>(width) / 2, static_cast<double>(height) / 2);
            int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
            if (focused)
            {
                context.m_horizontal_angle += context.m_view_speed * delta_time * (int(width / 2) - xpos);
                context.m_vertical_angle   += context.m_view_speed * delta_time * (int(height / 2) - ypos);
            }
        }
        glm::vec3 forward(
            cos(context.m_vertical_angle) * sin(context.m_horizontal_angle),
            sin(context.m_vertical_angle),
            cos(context.m_vertical_angle) * cos(context.m_horizontal_angle)
        );
        glm::vec3 right = glm::vec3(
            sin(context.m_horizontal_angle - 3.14f/2.0f),
            0,
            cos(context.m_horizontal_angle - 3.14f/2.0f)
        );
        glm::vec3 up = glm::cross(right, forward);

        context.m_show_mouse_cooldown += delta_time;
        if ((context.m_show_mouse_cooldown >= context.m_show_mouse_cooldown_max) && glfwGetKey(window, KEYMAP::SHOW_MOUSE) == GLFW_PRESS)
        {
            context.m_show_mouse_cooldown = 0;
            context.m_show_mouse = !context.m_show_mouse;
            glfwSetInputMode(window, GLFW_CURSOR, (context.m_show_mouse) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
            if (!context.m_show_mouse) { glfwSetCursorPos(window, static_cast<double>(width) / 2, static_cast<double>(height) / 2); }
        }
        if (glfwGetKey(window, KEYMAP::FORWARD) == GLFW_PRESS)
        {
            context.m_camera_xyz += forward * delta_time * context.m_move_speed;
        }
        if (glfwGetKey(window, KEYMAP::BACKWARD) == GLFW_PRESS)
        {
            context.m_camera_xyz -= forward * delta_time * context.m_move_speed;
        }
        if (glfwGetKey(window, KEYMAP::RIGHT) == GLFW_PRESS)
        {
            context.m_camera_xyz += right * delta_time * context.m_move_speed;
        }
        if (glfwGetKey(window, KEYMAP::LEFT) == GLFW_PRESS)
        {
            context.m_camera_xyz -= right * delta_time * context.m_move_speed;
        }
        return glm::lookAt(context.m_camera_xyz, context.m_camera_xyz + forward, up);
    }

    float get_fov()
    {
        return g_fov;
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        g_fov -= yoffset * 2;
    }
}