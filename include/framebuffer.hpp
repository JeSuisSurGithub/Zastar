#ifndef ZSLFRAMEBUFFER_HPP
#define ZSLFRAMEBUFFER_HPP

#include "common.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <memory>

namespace zsl
{
    namespace framebuffer
    {
        constexpr float SCREEN_VERTICES[] =
        {
            -1.0,  1.0,  0.0, 1.0,
            -1.0, -1.0,  0.0, 0.0,
             1.0, -1.0,  1.0, 0.0,

            -1.0,  1.0,  0.0, 1.0,
             1.0, -1.0,  1.0, 0.0,
             1.0,  1.0,  1.0, 1.0
        };

        constexpr float CLEAR_COLOR[4] = {0.0, 0.0, 0.0, 1.0};
        constexpr float CLEAR_DEPTH = 1.f;
        constexpr i32 CLEAR_STENCIL = 1;
        constexpr usz BLOOM_LEVEL = 2;

        typedef struct framebuffer
        {
            framebuffer(const framebuffer &) = delete;
            framebuffer &operator=(const framebuffer &) = delete;
            framebuffer(framebuffer &&) = delete;
            framebuffer &operator=(framebuffer &&) = delete;

            shader::shader m_final;
            shader::shader m_upsampler;
            shader::shader m_downsampler;

            GLuint m_vao;
            GLuint m_vbo;

            std::array<std::unique_ptr<texture::texture>, 2> m_colorbufs;
            GLuint m_fbo;

            texture::texture m_depthbuf;

            std::array<std::unique_ptr<texture::texture>, BLOOM_LEVEL> m_bloom_colorbufs;
            GLuint m_bloom_fbo;

            i32 m_width;
            i32 m_height;

            float m_time;

            framebuffer(i32 width, i32 height, u32 previous_count = 0);
            ~framebuffer();
        }framebuffer;

        void prepare_render(framebuffer& fb_);
        void end_render(framebuffer& fb_, float delta_time);
        void draw_quad(framebuffer& fb_);
    }
}

#endif /* ZSLFRAMEBUFFER_HPP */