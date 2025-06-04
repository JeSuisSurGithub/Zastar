#include <control.hpp>

#include <algorithm>

float g_fov = 45.0;

namespace zsl
{
namespace controls
{
    controls::controls(window::window& window_)
    :
    m_move_speed(0.1),
    m_view_speed(.0001),
    m_horizontal_angle(3.14),
    m_vertical_angle(0.0),
    m_show_mouse({.toggled = false, .waited_time = 0, .wait_time = 200, .key =  KEYMAP::SHOW_MOUSE}),
    m_freeze({.toggled = false, .waited_time = 0, .wait_time = 200, .key =  KEYMAP::FREEZE}),
    m_wireframe({.toggled = false, .waited_time = 0, .wait_time = 200, .key =  KEYMAP::WIREFRAME}),
    m_camera_xyz(glm::vec3(0.0, 0.0, 5.0))
    {
        set_scroll_callback(window_, scroll_callback);
    }

    controls::~controls() {}

    glm::mat4 process_controls(controls& context, window::window& window_, float delta_time)
    {
        window::window_size dimensions = window::get_size(window_);
        bool changed = update_toggle(context.m_show_mouse, window_, delta_time);
        update_toggle(context.m_freeze, window_, delta_time);
        update_toggle(context.m_wireframe, window_, delta_time);

        if (changed)
        {
            set_cursor_state(window_, context.m_show_mouse.toggled);
            if (!context.m_show_mouse.toggled)
            {
                set_cursor_pos(window_, {static_cast<float>(dimensions.x) / 2, static_cast<float>(dimensions.y) / 2});
            }
        }
        // Movement

        if (!context.m_show_mouse.toggled)
        {
            window::cursor_pos position = get_cursor_pos(window_);
            set_cursor_pos(window_, {static_cast<float>(dimensions.x) / 2, static_cast<float>(dimensions.y) / 2});
            if (is_focused(window_))
            {
                context.m_horizontal_angle +=
                    context.m_view_speed * sin(g_fov * 0.01) * delta_time * (int(dimensions.x / 2) - position.x);
                context.m_vertical_angle   +=
                    context.m_view_speed * sin(g_fov * 0.01) * delta_time * (int(dimensions.y / 2) - position.y);
            }
        }
        glm::vec3 forward(
            cos(context.m_vertical_angle) * sin(context.m_horizontal_angle),
            sin(context.m_vertical_angle),
            cos(context.m_vertical_angle) * cos(context.m_horizontal_angle));

        glm::vec3 right(
            sin(context.m_horizontal_angle - glm::pi<float>() / 2.0),
            0,
            cos(context.m_horizontal_angle - glm::pi<float>() / 2.0));

        glm::vec3 up = glm::cross(right, forward);
        if (is_pressed(window_, KEYMAP::INCREASE_SPEED))
        {
            context.m_move_speed += 0.05;
        }
        if (is_pressed(window_, KEYMAP::DECREASE_SPEED))
        {
            context.m_move_speed -= 0.05;
        }
        context.m_move_speed = std::clamp<float>(context.m_move_speed, 0.05, 2.0);
        if (is_pressed(window_, KEYMAP::FORWARD))
        {
            context.m_camera_xyz += forward * delta_time * context.m_move_speed;
        }
        if (is_pressed(window_, KEYMAP::BACKWARD))
        {
            context.m_camera_xyz -= forward * delta_time * context.m_move_speed;
        }
        if (is_pressed(window_, KEYMAP::RIGHT))
        {
            context.m_camera_xyz += right * delta_time * context.m_move_speed;
        }
        if (is_pressed(window_, KEYMAP::LEFT))
        {
            context.m_camera_xyz -= right * delta_time * context.m_move_speed;
        }
        return glm::lookAt(context.m_camera_xyz, context.m_camera_xyz + forward, up);
    }

    float get_fov()
    {
        return g_fov;
    }

    bool update_toggle(toggle& toggle_, window::window& window_, float delta_time)
    {
        toggle_.waited_time += delta_time;
        if ((toggle_.waited_time >= toggle_.wait_time) && is_pressed(window_, toggle_.key))
        {
            toggle_.waited_time = 0;
            toggle_.toggled = !toggle_.toggled;
            return true;
        }
        return false;
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        g_fov -= yoffset * 1.1;
        g_fov = std::clamp<float>(g_fov, 1.0, 300.0);
    }
}
}