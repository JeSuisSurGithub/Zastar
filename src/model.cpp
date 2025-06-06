#include <model.hpp>

#include <shader.hpp>
#include <ubo.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <ext/tiny_obj_loader.h>
#include <ext/stb_image.h>

namespace zsl
{
namespace model
{
    u64 model::s_next_id{1};

    model::model(const std::string& model_path)
    :
    m_id(s_next_id++)
    {
        load_obj(model_path);
        load_obj_to_gpu();
    }

    model::model(const std::string& model_path, std::shared_ptr<texture::texture> height_map)
    :
    m_id(s_next_id++)
    {
        load_obj(model_path);
        heightmap(m_vertices, height_map);
        load_obj_to_gpu();
    }

    void model::load_obj(const std::string& model_path)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path.c_str()))
            throw std::runtime_error(warn + err);

        std::unordered_map<vertex, uint32_t> unique_vertices{};

        for (const tinyobj::shape_t& shape : shapes)
        {
            for (const tinyobj::index_t& index : shape.mesh.indices)
            {
                vertex vertex{};
                vertex.xyz = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };

                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1 - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                if (unique_vertices.count(vertex) == 0) {
                    unique_vertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                    m_vertices.push_back(vertex);
                }

                m_indices.push_back(unique_vertices[vertex]);
            }
        }
    }

    void model::load_obj_to_gpu()
    {
        glCreateBuffers(1, &m_vbo);
        glNamedBufferStorage(m_vbo, m_vertices.size() * sizeof(vertex), m_vertices.data(),
            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        glCreateBuffers(1, &m_ebo);
        glNamedBufferStorage(m_ebo, m_indices.size() * sizeof(u32), m_indices.data(),
            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        glCreateVertexArrays(1, &m_vao);

        glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(vertex));
        glVertexArrayElementBuffer(m_vao, m_ebo);

        glEnableVertexArrayAttrib(m_vao, 0);
        glEnableVertexArrayAttrib(m_vao, 1);
        glEnableVertexArrayAttrib(m_vao, 2);

        glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, xyz));
        glVertexArrayAttribFormat(m_vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, normal));
        glVertexArrayAttribFormat(m_vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(vertex, uv));

        glVertexArrayAttribBinding(m_vao, 0, 0);
        glVertexArrayAttribBinding(m_vao, 1, 0);
        glVertexArrayAttribBinding(m_vao, 2, 0);
    }

    model::~model()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }

    void draw(model& model_)
    {
        glBindVertexArray(model_.m_vao);
        glDrawElements(GL_TRIANGLES, model_.m_indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }



    void heightmap(std::vector<vertex>& vertices, std::shared_ptr<texture::texture> height_map)
    {
        memory::ssbo ssbo_vertex{SSBO_BINDINGS::VERTEX, vertices.data(), vertices.size() * sizeof(vertex)};

        texture::bind(*height_map, 0);

        shader::shader heightmap_shader{"shaders/heightmap.comp", ZSL_LOAD_SPIRV};
        shader::bind(heightmap_shader);
        update_int(heightmap_shader, UNIFORM_LOCATIONS::VERTEX_COUNT, static_cast<int>(vertices.size()));
        update_int(heightmap_shader, UNIFORM_LOCATIONS::HEIGHTMAP, 0);

        GLuint local_size_x = 256;
        GLuint num_groups = (vertices.size() + local_size_x - 1) / local_size_x;
        glDispatchCompute(num_groups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glGetNamedBufferSubData(ssbo_vertex.m_ssbo, 0, vertices.size() * sizeof(vertex), vertices.data());
    }
}
}