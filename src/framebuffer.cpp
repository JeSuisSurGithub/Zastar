#include "texture.hpp"
#include <framebuffer.hpp>

namespace zsl
{
namespace framebuffer
{
    framebuffer::framebuffer(i32 width, i32 height, u32 previous_count)
    :
    m_final("shaders/quad.vert", "shaders/final.frag", ZSL_LOAD_SPIRV),
    m_upsampler("shaders/quad.vert", "shaders/upsampler.frag", ZSL_LOAD_SPIRV),
    m_downsampler("shaders/quad.vert", "shaders/downsampler.frag", ZSL_LOAD_SPIRV),
    m_depthbuf(width, height, 0, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_DEPTH24_STENCIL8),
    m_width(width),
    m_height(height),
    m_screen_tearing_count(previous_count)
    {
        glCreateBuffers(1, &m_vbo);
        glNamedBufferStorage(m_vbo, sizeof(SCREEN_VERTICES), &SCREEN_VERTICES, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        glCreateVertexArrays(1, &m_vao);
        glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, (4 * sizeof(float)));
        glEnableVertexArrayAttrib(m_vao, 0);
        glEnableVertexArrayAttrib(m_vao, 1);
        glVertexArrayAttribFormat(m_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE, (2 * sizeof(float)));
        glVertexArrayAttribBinding(m_vao, 0, 0);
        glVertexArrayAttribBinding(m_vao, 1, 0);

        glCreateFramebuffers(1, &m_fbo);
        for (usz index = 0; index < 2; index++)
        {
            m_colorbufs[index] =
                std::make_unique<texture::texture>(m_width, m_height, index, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGBA16F);
            bind_to_framebuffer(*m_colorbufs[index], m_fbo, GL_COLOR_ATTACHMENT0 + index);
        }

        // glCreateRenderbuffers(1, &m_rbo);
        // glNamedRenderbufferStorage(m_rbo, GL_DEPTH24_STENCIL8, m_width, m_height);
        // glNamedFramebufferRenderbuffer(m_fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
        bind_to_framebuffer(m_depthbuf, m_fbo, GL_DEPTH_STENCIL_ATTACHMENT);

        GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glNamedFramebufferDrawBuffers(m_fbo, 2, attachments);

        if (glCheckNamedFramebufferStatus(m_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Framebuffer incomplete\n");


        glm::vec2 mip_size{m_width, m_height};
        glCreateFramebuffers(1, &m_bloom_fbo);
        for (usz index = 0; index < BLOOM_LEVEL; index++)
        {
            mip_size /= 2.0;
            m_bloom_colorbufs[index] =
                std::make_unique<texture::texture>(mip_size.x, mip_size.y, index, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGBA16F);
        }
        glNamedFramebufferDrawBuffers(m_bloom_fbo, 1, attachments);
        bind_to_framebuffer(*m_bloom_colorbufs[0], m_bloom_fbo, GL_COLOR_ATTACHMENT0);
        if (glCheckNamedFramebufferStatus(m_bloom_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Framebuffer incomplete\n");

        update_int(m_upsampler, UNIFORM_LOCATIONS::UPSAMPLE_TEXTURE, 0);
        update_int(m_downsampler, UNIFORM_LOCATIONS::DOWNSAMPLE_TEXTURE, 0);
        update_int(m_final, UNIFORM_LOCATIONS::COMBINE_MAIN_SCENE, 0);
        update_int(m_final, UNIFORM_LOCATIONS::COMBINE_BLOOM, 1);
    }

    framebuffer::~framebuffer()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteFramebuffers(1, &m_fbo);
        // glDeleteRenderbuffers(1, &m_rbo);
        glDeleteFramebuffers(1, &m_bloom_fbo);
    }

    void prepare_render(framebuffer& fb_)
    {
        glClearNamedFramebufferfv(0, GL_COLOR, 0, CLEAR_COLOR);
        glClearNamedFramebufferfv(0, GL_DEPTH, 0, &CLEAR_DEPTH);
        glClearNamedFramebufferiv(0, GL_STENCIL, 0, &CLEAR_STENCIL);
        glClearNamedFramebufferfv(fb_.m_fbo, GL_COLOR, 0, CLEAR_COLOR);
        glClearNamedFramebufferfv(fb_.m_fbo, GL_DEPTH, 0, &CLEAR_DEPTH);
        glClearNamedFramebufferiv(fb_.m_fbo, GL_STENCIL, 0, &CLEAR_STENCIL);
        glClearNamedFramebufferfv(fb_.m_fbo, GL_COLOR, 1, CLEAR_COLOR);
        glBindFramebuffer(GL_FRAMEBUFFER, fb_.m_fbo);
    }

    void end_render(framebuffer& fb_, float delta_time)
    {
        fb_.m_screen_tearing_count += delta_time / 4.0;
        glBindFramebuffer(GL_FRAMEBUFFER, fb_.m_bloom_fbo);
        {
            bind(fb_.m_downsampler);
            update_vec2(fb_.m_downsampler,
                UNIFORM_LOCATIONS::SCREEN_RESOLUTION, {fb_.m_width, fb_.m_height});
            texture::bind(*fb_.m_colorbufs[1], 0);
            for (usz index = 0; index < BLOOM_LEVEL; index++)
            {
                glViewport(0, 0, fb_.m_bloom_colorbufs[index]->m_width, fb_.m_bloom_colorbufs[index]->m_height);
                bind_to_framebuffer(*fb_.m_bloom_colorbufs[index], fb_.m_bloom_fbo, GL_COLOR_ATTACHMENT0);
                draw_quad(fb_);
                update_vec2(fb_.m_downsampler,
                    UNIFORM_LOCATIONS::SCREEN_RESOLUTION, {fb_.m_bloom_colorbufs[index]->m_width, fb_.m_bloom_colorbufs[index]->m_height});
                texture::bind(*fb_.m_bloom_colorbufs[index], 0);
            }
        }
        {
            glBlendFunc(GL_ONE, GL_ONE);
            glBlendEquation(GL_FUNC_ADD);
            for (isz index = BLOOM_LEVEL - 1; index > 0; index--)
            {
                texture::bind(*fb_.m_bloom_colorbufs[index], 0);
                glViewport(0, 0, fb_.m_bloom_colorbufs[index - 1]->m_width, fb_.m_bloom_colorbufs[index - 1]->m_height);
                bind_to_framebuffer(*fb_.m_bloom_colorbufs[index - 1], fb_.m_bloom_fbo, GL_COLOR_ATTACHMENT0);
                draw_quad(fb_);
            }
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, fb_.m_width, fb_.m_height);

        bind(fb_.m_final);
        update_vec2(fb_.m_final,
            UNIFORM_LOCATIONS::SCREEN_RESOLUTION, {fb_.m_width, fb_.m_height});
        update_uint(fb_.m_final, UNIFORM_LOCATIONS::SCREEN_TEARING_SCAN_POS, fb_.m_screen_tearing_count);
        texture::bind(*fb_.m_colorbufs[0], 0);
        texture::bind(*fb_.m_bloom_colorbufs[0], 1);
        draw_quad(fb_);
    }

    void draw_quad(framebuffer& fb_)
    {
        glBindVertexArray(fb_.m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}
}