#ifndef ZSLCONTROL_HPP
#define ZSLCONTROL_HPP

#include "common.hpp"
#include "window.hpp"

namespace zsl
{
    namespace controls
    {
        typedef enum KEYMAP
        {
            FORWARD         = GLFW_KEY_W,
            LEFT            = GLFW_KEY_A,
            BACKWARD        = GLFW_KEY_S,
            RIGHT           = GLFW_KEY_D,
            EXIT            = GLFW_KEY_ESCAPE,
            SHOW_MOUSE      = GLFW_KEY_LEFT_ALT,
            SPEED           = GLFW_KEY_LEFT_SHIFT,
            FREEZE          = GLFW_KEY_SPACE,
            WIREFRAME       = GLFW_KEY_LEFT_CONTROL,
            INCREASE_SPEED  = GLFW_KEY_UP,
            DECREASE_SPEED  = GLFW_KEY_DOWN,
        }KEYMAP;

        typedef struct toggle
        {
            bool toggled;
            u32 waited_time;
            u32 wait_time;
            KEYMAP key;
        }toggle;

        typedef struct controls
        {
            controls(const controls &) = delete;
            controls &operator=(const controls &) = delete;
            controls(controls &&) = delete;
            controls &operator=(controls &&) = delete;

            float m_move_speed;
            float m_view_speed;
            float m_horizontal_angle;
            float m_vertical_angle;

            toggle m_show_mouse;
            toggle m_freeze;
            toggle m_wireframe;
            glm::vec3 m_camera_xyz;

            controls(window::window& window_);
            ~controls();
        }controls;

        glm::mat4 process_controls(controls& context, window::window& window_, float delta_time, glm::vec3& forward);

        float get_fov();

        bool update_toggle(toggle& toggle_, window::window& window_, float delta_time);

        void scroll_callback(GLFWwindow* window_, double xoffset, double yoffset);
    }
}

#endif /* ZSLCONTROL_HPP */