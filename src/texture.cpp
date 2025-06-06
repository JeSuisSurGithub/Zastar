#include <texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <ext/stb_image.h>

#include <stdexcept>

namespace zsl
{
namespace texture
{
    u64 texture::s_next_id{1};

    texture::texture(u32 width, u32 height, GLenum filter, GLenum wrap, GLenum format)
    :
    m_id(s_next_id++),
    m_width(width),
    m_height(height)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_tex);
        glTextureParameteri(m_tex, GL_TEXTURE_MIN_FILTER, filter);
        glTextureParameteri(m_tex, GL_TEXTURE_MAG_FILTER, filter);
        glTextureParameteri(m_tex, GL_TEXTURE_WRAP_S, wrap);
        glTextureParameteri(m_tex, GL_TEXTURE_WRAP_T, wrap);
        glTextureStorage2D(m_tex, 1, format, m_width, m_height);
    }

    texture::texture(const std::string& texture_path, GLenum wrap)
    :
    m_id(s_next_id++)
    {
        int width;
        int height;
        int channels;
        stbi_uc* data = stbi_load(texture_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!data) {
            throw std::invalid_argument("Could not load texture\n");
        }

        m_width = width;
        m_height = height;
        glCreateTextures(GL_TEXTURE_2D, 1, &m_tex);
        glTextureParameteri(m_tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_tex, GL_TEXTURE_WRAP_S, wrap);
        glTextureParameteri(m_tex, GL_TEXTURE_WRAP_T, wrap);
        glTextureParameteri(m_tex, GL_TEXTURE_MAX_ANISOTROPY, 16.f);
        glTextureStorage2D(m_tex, 1, GL_RGBA8, m_width, m_height);
        glTextureSubImage2D(m_tex, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(m_tex);
        stbi_image_free(data);
    }

    texture::~texture()
    {
        glDeleteTextures(1, &m_tex);
    }

    void bind(texture& texture_, u32 index)
    {
        glBindTextureUnit(index, texture_.m_tex);
    }

    void bind_to_framebuffer(texture& texture_, GLuint framebuffer, GLuint attachment)
    {
        glNamedFramebufferTexture(framebuffer, attachment, texture_.m_tex, 0);
    }
}
}