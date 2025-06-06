#ifndef ZSLMODEL_HPP
#define ZSLMODEL_HPP

#include "common.hpp"
#include "texture.hpp"
#include <memory>

namespace std
{
    template<> struct hash<zsl::vertex>
    {
        std::size_t operator()(zsl::vertex const& vertex) const
        {
            return
                ((hash<glm::vec3>()(vertex.xyz) ^
                (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
}

namespace zsl
{
    namespace model
    {
        typedef struct model
        {
            model(const model &) = delete;
            model &operator=(const model &) = delete;
            model(model &&) = delete;
            model &operator=(model &&) = delete;

            static u64 s_next_id;
            u64 m_id;

            std::vector<vertex> m_vertices;
            std::vector<u32> m_indices;

            GLuint m_vbo;
            GLuint m_vao;
            GLuint m_ebo;

            model(const std::string& model_path);
            model(const std::string& model_path, std::shared_ptr<texture::texture> height_map);
            ~model();

            void load_obj(const std::string& model_path);
            void load_obj_to_gpu();
        }model;

        void heightmap(std::vector<vertex>& vertices, std::shared_ptr<texture::texture> height_map);

        void draw(model& model_);
    }
}

#endif /* ZSLMODEL_HPP */