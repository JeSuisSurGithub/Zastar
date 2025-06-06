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
    m_depthbuf(width, height, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_DEPTH24_STENCIL8),
    m_width(width),
    m_height(height),
    m_time(previous_count)
    {
        glCreateBuffers(1, &m_vbo);
        glNamedBufferStorage(m_vbo, sizeof(FULL_QUAD), &FULL_QUAD, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

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
                std::make_unique<texture::texture>(m_width, m_height, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA16F);
            bind_to_framebuffer(*m_colorbufs[index], m_fbo, GL_COLOR_ATTACHMENT0 + index);
        }

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
                std::make_unique<texture::texture>(mip_size.x, mip_size.y, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGBA16F);
        }
        glNamedFramebufferDrawBuffers(m_bloom_fbo, 1, attachments);
        bind_to_framebuffer(*m_bloom_colorbufs[0], m_bloom_fbo, GL_COLOR_ATTACHMENT0);
        if (glCheckNamedFramebufferStatus(m_bloom_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Framebuffer incomplete\n");

        update_int(m_upsampler, UNIFORM_LOCATIONS::UPSAMPLE_TEXTURE, 0);
        update_int(m_downsampler, UNIFORM_LOCATIONS::DOWNSAMPLE_TEXTURE, 0);
        update_int(m_final, UNIFORM_LOCATIONS::PLAIN_IMAGE, 0);
        update_int(m_final, UNIFORM_LOCATIONS::BLOOM_IMAGE, 1);
        update_int(m_final, UNIFORM_LOCATIONS::DEPTH_STENCIL_TEXTURE, 2);
    }

    framebuffer::~framebuffer()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteFramebuffers(1, &m_fbo);
        glDeleteFramebuffers(1, &m_bloom_fbo);
    }

    void prepare_fb(framebuffer& fb_)
    {
        glClearNamedFramebufferfv(0, GL_COLOR, 0, CLEAR_COLOR);
        glClearNamedFramebufferfv(0, GL_DEPTH, 0, &CLEAR_DEPTH);
        glClearNamedFramebufferiv(0, GL_STENCIL, 0, &CLEAR_STENCIL);
        glClearNamedFramebufferfv(fb_.m_fbo, GL_COLOR, 0, CLEAR_COLOR);
        glClearNamedFramebufferfv(fb_.m_fbo, GL_DEPTH, 0, &CLEAR_DEPTH);
        glClearNamedFramebufferiv(fb_.m_fbo, GL_STENCIL, 0, &CLEAR_STENCIL);
        glClearNamedFramebufferfv(fb_.m_fbo, GL_COLOR, 1, CLEAR_COLOR);
        glBindFramebuffer(GL_FRAMEBUFFER, fb_.m_fbo);
        glViewport(0, 0, fb_.m_width, fb_.m_height);
    }

    void render_w_fx(framebuffer& fb_, float delta_time)
    {
        fb_.m_time += delta_time;
        glBindFramebuffer(GL_FRAMEBUFFER, fb_.m_bloom_fbo);
        {
            bind(fb_.m_downsampler);
            texture::bind(*fb_.m_colorbufs[1], 0);
            for (usz index = 0; index < BLOOM_LEVEL; index++)
            {
                glViewport(0, 0, fb_.m_bloom_colorbufs[index]->m_width, fb_.m_bloom_colorbufs[index]->m_height);
                bind_to_framebuffer(*fb_.m_bloom_colorbufs[index], fb_.m_bloom_fbo, GL_COLOR_ATTACHMENT0);
                draw_quad(fb_);
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
        // update_float(fb_.m_final, UNIFORM_LOCATIONS::TIME, fb_.m_time);
        texture::bind(*fb_.m_colorbufs[0], 0);
        texture::bind(*fb_.m_bloom_colorbufs[0], 1);
        texture::bind(fb_.m_depthbuf, 2);
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