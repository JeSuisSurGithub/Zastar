#include <rendergroups/stargroup.hpp>

namespace zsl
{

namespace rendergroups
{
    star::star(
        rendergroup& group,
        const std::string& model_path,
        const std::string& texture_path,
        glm::vec3 position,
        glm::vec3 rotation,
        glm::vec3 scale,
        glm::vec3 color,
        glm::vec3 range,
        usz planet_count_)
    :
    base(create_object(group, model_path, texture_path))
    {
        base.m_translation = position;
        base.m_euler_angles = rotation;
        base.m_scale = scale;
        point_light.color = color;
        point_light.position = position;
        point_light.range = range;
        planet_count = planet_count_;
        texture_offset_count = 0.0;
    }

    star::~star() {}

    stargroup::stargroup()
    :
    m_ubo(UBO_BINDINGS::STAR, nullptr, sizeof(ubo_star))
    {
        m_base = std::make_unique<rendergroup>("shaders/stars.vert", "shaders/stars.frag");
    }

    stargroup::~stargroup() {}

    void update(stargroup& context, float delta_time)
    {
        for (star& cur_object : context.m_stars)
        {
            cur_object.texture_offset_count += delta_time * 0.000015;
        }
    }

    void render(stargroup& context, glm::vec3 camera_xyz)
    {
        bind(*context.m_base->m_program);
        ubo_star cur_ubo;
        for (const star& cur_object : context.m_stars)
        {
            if (glm::distance(camera_xyz, cur_object.base.m_translation) > ZFAR * 0.65)
                continue;
            bind(*context.m_base->m_textures[cur_object.base.m_texture_index], cur_object.base.m_texture_index);
            cur_ubo.transform = get_transform_mat(cur_object.base);
            cur_ubo.inverse_transform = glm::inverse(cur_ubo.transform);
            cur_ubo.texture_index = cur_object.base.m_texture_index;
            cur_ubo.texture_offset = cur_object.texture_offset_count;
            memory::update(context.m_ubo, &cur_ubo, sizeof(ubo_star), 0);
            draw(*context.m_base->m_models[cur_object.base.m_model_index]);
        }
    }

    glm::vec3 light_range_constants(float lightrange)
    {
        return glm::vec3(1.0, 4.5 / lightrange, 75.0 / (lightrange * lightrange));
    }
}

}