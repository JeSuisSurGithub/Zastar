#ifndef ZSLTEXTURE_HPP
#define ZSLTEXTURE_HPP

#include "common.hpp"

namespace zsl
{
    namespace texture
    {
        typedef struct texture
        {
            texture(const texture &) = delete;
            texture &operator=(const texture &) = delete;
            texture(texture &&) = delete;
            texture &operator=(texture &&) = delete;

            const std::string m_texture_path;
            GLuint m_id;
            u32 m_index;
            i32 m_width;
            i32 m_height;

            texture(u32 width, u32 height, u32 index, GLenum filter, GLenum wrap, GLenum format);
            texture(const std::string& texture_path, u32 index,
                GLenum S_WRAP = GL_REPEAT, GLenum T_WRAP = GL_REPEAT);
            ~texture();
        }texture;

        void bind(texture& texture_);
        void bind(texture& texture_, u32 index);
        void bind_to_framebuffer(texture& texture_, GLuint framebuffer, GLuint attachment);
    }
}

#endif /* ZSLTEXTURE_HPP */