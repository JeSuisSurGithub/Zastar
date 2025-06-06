#ifndef TEXTGROUP_HPP
#define TEXTGROUP_HPP

#include "common.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace zsl
{
    namespace rendergroups
    {
        typedef struct text
        {
            std::string text;
            glm::vec2 xy;
        }text;

        typedef struct textgroup
        {
            textgroup(const textgroup &) = delete;
            textgroup &operator=(const textgroup &) = delete;
            textgroup(textgroup &&) = delete;
            textgroup &operator=(textgroup &&) = delete;

            const usz m_size;
            std::vector<glm::vec4> m_vertices;
            std::vector<glm::vec4> m_translations;
            std::vector<GLint> m_firsts;
            std::vector<GLsizei> m_counts;

            GLuint m_vao;
            GLuint m_vbo;

            texture::texture m_characters;
            shader::shader m_shader;

            textgroup(usz size);
            ~textgroup();
        }textgroup;

        void render(textgroup& group, glm::vec2 framebuffer_size, const std::vector<text>& texts);
    }
}

#endif /* TEXTGROUP_HPP */