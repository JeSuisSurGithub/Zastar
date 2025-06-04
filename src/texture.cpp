#include "common.hpp"
#include <texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <ext/stb_image.h>

namespace zsl
{
namespace texture
{
    texture::texture(u32 width, u32 height, GLenum filter, GLenum wrap, GLenum format)
    :
    m_width(width),
    m_height(height)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, filter);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, filter);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, wrap);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, wrap);
        glTextureStorage2D(m_id, 1, format, m_width, m_height);
    }

    texture::texture(const std::string& texture_path, GLenum wrap)
    :
    m_texture_path(texture_path)
    {
        int width;
        int height;
        int channels;
        stbi_uc* data = stbi_load(m_texture_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!data) { throw std::invalid_argument("Could not load texture\n"); }

        m_width = width;
        m_height = height;
        glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, wrap);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, wrap);
        glTextureParameteri(m_id, GL_TEXTURE_MAX_ANISOTROPY, 16.f);
        glTextureStorage2D(m_id, 1, GL_RGBA8, m_width, m_height);
        glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(m_id);
        stbi_image_free(data);
    }

    texture::~texture()
    {
        glDeleteTextures(1, &m_id);
    }

    void bind(texture& texture_, u32 index)
    {
        glBindTextureUnit(index, texture_.m_id);
    }

    void bind_to_framebuffer(texture& texture_, GLuint framebuffer, GLuint attachment)
    {
        glNamedFramebufferTexture(framebuffer, attachment, texture_.m_id, 0);
    }
}
}