#ifndef ZSLCTX_HPP
#define ZSLCTX_HPP

#include "common.hpp"
#include "control.hpp"
#include "rendergroups/planetgroup.hpp"
#include "rendergroups/stargroup.hpp"
#include "rendergroups/textgroup.hpp"
#include "framebuffer.hpp"
#include "gen.hpp"
#include "ubo.hpp"
#include "window.hpp"

#include <memory>

namespace zsl
{
    constexpr usz TEXT_SIZE = 20;
    typedef struct ctx
    {
        ctx(const ctx &) = delete;
        ctx &operator=(const ctx &) = delete;
        ctx(ctx &&) = delete;
        ctx &operator=(ctx &&) = delete;

        window::window m_window;
        controls::controls m_controls;

        std::unique_ptr<framebuffer::framebuffer> m_framebuffer;
        std::unique_ptr<rendergroups::stargroup> m_stargroup;
        std::unique_ptr<rendergroups::planetgroup> m_planetgroup;
        std::unique_ptr<rendergroups::textgroup> m_textgroup;
        std::unique_ptr<memory::ubo> m_ubo;

        ctx(bool opengl_debug, u32 seed, usz size_count);
        ~ctx();
    }ctx;

    void GLAPIENTRY opengl_debug_callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* user_param);

    void run(ctx& ctx_);
}

#endif /* ZSLCTX_HPP */