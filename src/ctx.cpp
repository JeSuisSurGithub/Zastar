#include <sstream>
#include <ctx.hpp>

#include <chrono>
#include <iostream>

namespace zsl
{
    void GLAPIENTRY opengl_debug_callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* user_param)
    {
        if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::cout << "---------------" << std::endl;
        std::cout << "Debug message (" << id << "): " <<  message << std::endl;

        switch (source)
        {
            case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
            case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
        } std::cout << std::endl;

        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
            case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
        } std::cout << std::endl;

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
            case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
            case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
        } std::cout << std::endl;
        std::cout << std::endl;
    }

    ctx::ctx(bool opengl_debug, u32 seed)
    :
    m_window(),
    m_controls(m_window)
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD\n");
        }

        GLint major;
        GLint minor;
        GLint ext_count;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        glGetIntegerv(GL_NUM_EXTENSIONS, &ext_count);

        std::cout << "OpenGL VERSION: " << major << '.' << minor << std::endl;
        std::cout << "VENDOR: " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "RENDERER: " << glGetString(GL_RENDERER) << std::endl;
        std::cout << "EXTENSIONS:" << std::endl;
        for (usz count = 0; count < ext_count; count++)
            std::cout << '\t' << glGetStringi(GL_EXTENSIONS, count) << std::endl;
        std::cout << "SEED: " << seed << std::endl;

        if (opengl_debug)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(opengl_debug_callback, 0);
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        glStencilFunc(GL_EQUAL, 1, 0xFF);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        window::window_size dimensions = get_size(m_window);
        m_framebuffer = std::make_unique<framebuffer::framebuffer>(dimensions.x, dimensions.y);
        m_stargroup = std::make_unique<rendergroups::stargroup>();
        m_planetgroup = std::make_unique<rendergroups::planetgroup>();
        m_textgroup = std::make_unique<rendergroups::textgroup>(TEXT_SIZE);

        generate(*m_stargroup, *m_planetgroup, seed, 32);
        ubo_shared ubo_point_lights;
        for (ubo_point_lights.point_light_count = 0; ubo_point_lights.point_light_count < m_stargroup->m_stars.size(); ubo_point_lights.point_light_count++)
        {
            ubo_point_lights.point_lights[ubo_point_lights.point_light_count]
                = m_stargroup->m_stars[ubo_point_lights.point_light_count].point_light;
        }
        m_ubo = std::make_unique<memory::ubo>(UBO_BINDINGS::SHARED, (void*)&ubo_point_lights, ubo_shared_size);
    }

    ctx::~ctx() {}

    void run(ctx& ctx_)
    {
        ubo_shared cur_ubo;
        u32 frame_count = 0;
        std::string scroll_text = "/!\\ !!! TEST SCROLLING LONG MESSAGE !!! /!\\";
        glm::vec2 scroll_text_xy = {0, TEXT_SIZE * 4};

        std::chrono::system_clock::time_point cur_time = std::chrono::high_resolution_clock::now();
        while (!should_close(ctx_.m_window))
        {
            std::chrono::system_clock::time_point new_time = std::chrono::high_resolution_clock::now();
            float delta_time =
                std::chrono::duration<float, std::chrono::milliseconds::period>(new_time - cur_time).count();
            cur_time = new_time;

            update(ctx_.m_window);
            cur_ubo.view = process_controls(ctx_.m_controls, ctx_.m_window, delta_time);

            window::window_size dimensions = get_size(ctx_.m_window);
            cur_ubo.projection =
                glm::perspective<float>(glm::radians(controls::get_fov()), (float)dimensions.x / (float)dimensions.y, ZNEAR, ZFAR);
            cur_ubo.camera_xyz = ctx_.m_controls.m_camera_xyz;

            if (dimensions.x != ctx_.m_framebuffer->m_width || dimensions.y != ctx_.m_framebuffer->m_height)
            {
                glViewport(0, 0, dimensions.x, dimensions.y);
                u32 previous_count = ctx_.m_framebuffer->m_time;
                ctx_.m_framebuffer = std::make_unique<framebuffer::framebuffer>(dimensions.x, dimensions.y, previous_count);
            }

            rendergroups::update(*ctx_.m_stargroup, delta_time);
            if (!ctx_.m_controls.m_freeze.toggled)
                { rendergroups::update(*ctx_.m_planetgroup, delta_time, ctx_.m_stargroup->m_stars); }
            memory::update(*ctx_.m_ubo, &cur_ubo, offsetof(ubo_shared, point_lights), 0);
            std::vector<rendergroups::text> texts(5);
            std::stringstream tmp;

            tmp <<  "FPS: " << 1 / (delta_time * 0.001);
            texts[0] = {.text = tmp.str(), .xy = glm::vec2(0.0, TEXT_SIZE * 0)};
            tmp.str(std::string());

            tmp
                << "X: " << glm::round(ctx_.m_controls.m_camera_xyz.x)
                << " Y: " << glm::round(ctx_.m_controls.m_camera_xyz.y)
                << " Z: " << glm::round(ctx_.m_controls.m_camera_xyz.z);
            texts[1] = {.text = tmp.str(), .xy = glm::vec2(0.0, TEXT_SIZE * 1)};
            tmp.str(std::string());

            tmp << "FOV: " << controls::get_fov() << " & SPEED: " << ctx_.m_controls.m_move_speed;
            texts[2] = {.text = tmp.str(), .xy = glm::vec2(0.0, TEXT_SIZE * 2)};
            tmp.str(std::string());
            tmp
                << "RESOLUTION: " << dimensions.x << 'x' << dimensions.y
                << " FRAMES: " << frame_count;
            texts[3] = {.text = tmp.str(), .xy = glm::vec2(0.0, TEXT_SIZE * 3)};

            texts[4] = {.text = scroll_text, .xy = scroll_text_xy};
            scroll_text_xy.x = scroll_text_xy.x - delta_time * 0.2;

            if (scroll_text_xy.x < -((isz)TEXT_SIZE * (isz)scroll_text.size()))
                scroll_text_xy.x = dimensions.x;

            prepare_render(*ctx_.m_framebuffer);
                if (ctx_.m_controls.m_wireframe.toggled) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); };
                rendergroups::render(*ctx_.m_stargroup, cur_ubo.camera_xyz);
                rendergroups::render(*ctx_.m_planetgroup, cur_ubo.camera_xyz);
                if (ctx_.m_controls.m_wireframe.toggled) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); };
                rendergroups::render(*ctx_.m_textgroup, dimensions, texts);
            end_render(*ctx_.m_framebuffer, delta_time);

            swap_buffers(ctx_.m_window);
            frame_count++;

            do
            {
                new_time = std::chrono::high_resolution_clock::now();
                delta_time =
                    std::chrono::duration<float, std::chrono::microseconds::period>(new_time - cur_time).count();
            }
            while (delta_time < 16666);
        }
    }
}