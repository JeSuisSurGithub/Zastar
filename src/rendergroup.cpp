#include <rendergroup.hpp>

#include <algorithm>

#include <glm/gtx/quaternion.hpp>

namespace zsl
{
namespace rendergroups
{
    object::object(u32 model_index, u32 texture_index)
    :
    m_model_index(model_index),
    m_texture_index(texture_index),
    m_translation({0.0, 0.0, 0.0}),
    m_euler_angles({0.0, 0.0, 0.0}),
    m_scale({1.0, 1.0, 1.0})
    {
    }

    object::~object() {}

    void translate(object& object_, glm::vec3 translation) { object_.m_translation += translation; }
    void rotate(object& object_, glm::vec3 euler_angles) { object_.m_euler_angles += euler_angles; }
    void scale(object& object_, glm::vec3 scale) { object_.m_scale *= scale; }

    glm::mat4 get_transform_mat(const object& object_)
    {
        glm::mat4 translation_mat = glm::translate(glm::mat4(1.0), object_.m_translation);
        glm::mat4 rotation_mat = glm::toMat4(glm::quat(object_.m_euler_angles));
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0), object_.m_scale);
        return translation_mat * rotation_mat * scale_mat;
    }

    rendergroup::rendergroup(const std::string& vert_path, const std::string& frag_path)
    :
    m_program(std::make_unique<shader::shader>(vert_path, frag_path, ZSL_LOAD_SPIRV))
    {
        GLint values[MAX_TEXTURE_COUNT];
        for (usz index = 0; index < MAX_TEXTURE_COUNT; index++)
            values[index] = index;
        update_int_array(*m_program, UNIFORM_LOCATIONS::TEXTURE, values, MAX_TEXTURE_COUNT);
    }

    rendergroup::~rendergroup() {}

    object create_object(rendergroup& group, const std::string& model_path, const std::string& texture_path)
    {
        u32 model_index = add_model(group, model_path);
        u32 texture_index = add_texture(group, texture_path);
        return create_object(model_index, texture_index);
    }

    object create_object(
        rendergroup& group,
        const std::string& model_path,
        const std::string& texture_path,
        const std::string& height_map_path)
    {
        u32 model_index = add_model(group, model_path, height_map_path);
        u32 texture_index = add_texture(group, texture_path);
        return create_object(model_index, texture_index);
    }

    object create_object(u32 model_index, u32 texture_index)
    {
        return object(model_index, texture_index);
    }

    u32 add_model(rendergroup& group, const std::string& model_path)
    {
        std::vector<std::unique_ptr<model::model>>::iterator iterator = std::find_if(
            group.m_models.begin(),
            group.m_models.end(),
            [&](std::unique_ptr<model::model>& model) -> bool {
            return (model->m_model_path == model_path);
        });

        if (iterator == std::end(group.m_models)) {
            group.m_models.push_back(std::make_unique<model::model>(model_path));
            return group.m_models.size() - 1;
        }
        return std::distance(std::begin(group.m_models), iterator);
    }

    u32 add_model(
        rendergroup& group,
        const std::string& model_path,
        const std::string& height_map_path)
    {
        std::vector<std::unique_ptr<model::model>>::iterator iterator = std::find_if(
            group.m_models.begin(),
            group.m_models.end(),
            [&](std::unique_ptr<model::model>& model) -> bool
        {
            return (model->m_model_path == model_path) && (model->m_height_map_path == height_map_path);
        });

        if (iterator == std::end(group.m_models)) {
            group.m_models.push_back(std::make_unique<model::model>(model_path, height_map_path));
            return group.m_models.size() - 1;
        }
        return std::distance(std::begin(group.m_models), iterator);
    }

    u32 add_texture(rendergroup& group, const std::string& texture_path)
    {
        if (group.m_textures.size() == MAX_TEXTURE_COUNT)
            throw std::logic_error("Maximum texture count reached");

        std::vector<std::unique_ptr<texture::texture>>::iterator iterator = std::find_if(
            group.m_textures.begin(),
            group.m_textures.end(),
            [&](std::unique_ptr<texture::texture>& texture) -> bool
        {
            return (texture->m_texture_path == texture_path);
        });

        if (iterator == std::end(group.m_textures)) {
            u32 texture_index = group.m_textures.size();
            group.m_textures.push_back(std::make_unique<texture::texture>(texture_path));
            return texture_index;
        }
        return std::distance(std::begin(group.m_textures), iterator);
    }
}
}