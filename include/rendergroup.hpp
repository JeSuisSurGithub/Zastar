#ifndef ZSLRENDERGROUPS_HPP
#define ZSLRENDERGROUPS_HPP

#include "shader.hpp"
#include "texture.hpp"
#include "model.hpp"

#include <memory>

namespace zsl
{
    namespace rendergroups
    {
        typedef struct object
        {
            u32 m_model_index;
            u32 m_texture_index;

            glm::vec3 m_translation;
            glm::vec3 m_euler_angles;
            glm::vec3 m_scale;

            object(u32 model_index, u32 texture_index);
            ~object();
        }object;

        void translate(object& object_, glm::vec3 translation);
        void rotate(object& object_, glm::vec3 euler_angles);
        void scale(object& object_, glm::vec3 scale);
        glm::mat4 get_transform_mat(const object& object_);

        typedef struct rendergroup
        {
            rendergroup(const rendergroup &) = delete;
            rendergroup &operator=(const rendergroup &) = delete;
            rendergroup(rendergroup &&) = delete;
            rendergroup &operator=(rendergroup &&) = delete;

            shader::shader m_shader;
            // Beware if writing remove texture
            std::vector<std::shared_ptr<texture::texture>> m_textures;
            std::vector<std::shared_ptr<model::model>> m_models;

            rendergroup(const std::string& vert_path, const std::string& frag_path);
            ~rendergroup();
        }rendergroup;

        object create_object(u32 model_index, u32 texture_index);

        object create_object(
            rendergroup& group,
            std::shared_ptr<model::model> model,
            std::shared_ptr<texture::texture> tex);

        u32 add_model(rendergroup& group, std::shared_ptr<model::model> model);

        u32 add_texture(rendergroup& group, std::shared_ptr<texture::texture> tex);
    }
}

#endif /* ZSLRENDERGROUPS_HPP */