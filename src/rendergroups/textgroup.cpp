#include <rendergroups/textgroup.hpp>
#include <ubo.hpp>

namespace zsl
{
namespace rendergroups
{
    textgroup::textgroup(usz size)
    :
    m_size(size),
    m_characters("textures/font.png", GL_CLAMP_TO_EDGE),
    m_program("shaders/text.vert", "shaders/text.frag", ZSL_LOAD_SPIRV)
    {
        update_int(m_program, UNIFORM_LOCATIONS::CHARACTER_TEXTURE, 0);

        for (usz count = 0; count < 256; count++)
        {
            glm::vec2 xy_upleft    = glm::vec2(0 , 0);
            glm::vec2 xy_upright   = glm::vec2(m_size, 0);
            glm::vec2 xy_downright = glm::vec2(m_size, m_size);
            glm::vec2 xy_downleft  = glm::vec2(0, m_size);

            char c = count;
            float uv_x = (c % 16) / 16.0;
            float uv_y = (c / 16) / 16.0;

            glm::vec2 uv_upleft    = glm::vec2(uv_x             , uv_y);
            glm::vec2 uv_upright   = glm::vec2(uv_x + 1.0 / 16.0, uv_y);
            glm::vec2 uv_downright = glm::vec2(uv_x + 1.0 / 16.0, (uv_y + 1.0 / 16.0));
            glm::vec2 uv_downleft  = glm::vec2(uv_x             , (uv_y + 1.0 / 16.0));

            m_vertices.push_back(glm::vec4(xy_upleft, uv_upleft));
            m_vertices.push_back(glm::vec4(xy_downleft, uv_downleft));
            m_vertices.push_back(glm::vec4(xy_upright, uv_upright));

            m_vertices.push_back(glm::vec4(xy_downright, uv_downright));
            m_vertices.push_back(glm::vec4(xy_upright, uv_upright));
            m_vertices.push_back(glm::vec4(xy_downleft, uv_downleft));
        }

        glCreateBuffers(1, &m_vbo);
        glNamedBufferStorage(m_vbo, m_vertices.size() * sizeof(glm::vec4), m_vertices.data(), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        glCreateVertexArrays(1, &m_vao);
        glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, (4 * sizeof(float)));
        glEnableVertexArrayAttrib(m_vao, 0);
        glEnableVertexArrayAttrib(m_vao, 1);
        glVertexArrayAttribFormat(m_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE, (2 * sizeof(float)));
        glVertexArrayAttribBinding(m_vao, 0, 0);
        glVertexArrayAttribBinding(m_vao, 1, 0);
    }

    textgroup::~textgroup()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
    }

    void render(textgroup& group, glm::vec2 framebuffer_size, const std::vector<text>& texts)
    {
        bind(group.m_program);
        bind(group.m_characters, 0);
        update_vec2(group.m_program, UNIFORM_LOCATIONS::SCREEN_RESOLUTION, framebuffer_size);

        group.m_translations.clear();
        group.m_firsts.clear();
        group.m_counts.clear();

        for (const text& cur_text : texts)
        {
            for (usz count = 0; count < cur_text.text.size(); count++) {
                group.m_translations.push_back(glm::vec4(group.m_size * count + cur_text.xy.x, cur_text.xy.y, 0, 0));
                group.m_firsts.push_back(cur_text.text[count] * 6);
                group.m_counts.push_back(6);
            }
        }

        memory::ssbo ssbo_text(SSBO_BINDINGS::TEXT, group.m_translations.data(),
            group.m_translations.size() * sizeof(glm::vec4));
        glBindVertexArray(group.m_vao);
        glMultiDrawArrays(GL_TRIANGLES, group.m_firsts.data(), group.m_counts.data(), group.m_translations.size());
        glBindVertexArray(0);
    }
}
}