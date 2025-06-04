#include <rendergroups/planetgroup.hpp>

namespace zsl
{

namespace rendergroups
{
    planet::planet(
        rendergroup& group,
        const std::string& model_path,
        const std::string& texture_path,
        const std::string& height_map_path,
        glm::vec3 position,
        glm::vec3 rotation,
        glm::vec3 scale,
        material material__,
        float planet_distance_to_star_,
        float planet_revolution_speed_,
        float planet_orbital_speed_,
        float cur_angle_)
    :
    base(create_object(group, model_path, texture_path, height_map_path))
    {
        base.m_translation = position;
        base.m_euler_angles = rotation;
        base.m_scale = scale;
        material_ = material__;
        planet_distance_to_star = planet_distance_to_star_;
        planet_revolution_speed = planet_revolution_speed_;
        planet_orbital_speed = planet_orbital_speed_;
        cur_angle = cur_angle_;
    }

    planet::~planet() {}

    planetgroup::planetgroup()
    :
    m_ubo(UBO_BINDINGS::PLANET, nullptr, sizeof(ubo_planet))
    {
        m_base = std::make_unique<rendergroup>("shaders/planets.vert", "shaders/planets.frag");
    }

    planetgroup::~planetgroup() {}

    void update(planetgroup& context, float delta_time, const std::vector<star>& stars)
    {
        usz planet_count = 0;
        for (usz star_count = 0; star_count < stars.size(); star_count++)
        {
            glm::vec3 position = stars[star_count].base.m_translation;
            for (usz index = 0; index < stars[star_count].planet_count; index++, planet_count++)
            {
                planet& shorthand = context.m_planets[planet_count];

                context.m_planets[planet_count].cur_angle += delta_time * shorthand.planet_orbital_speed;
                rotate(context.m_planets[planet_count].base,
                    glm::vec3(0.0, delta_time * shorthand.planet_revolution_speed, 0.0));
                shorthand.base.m_translation = {
                    position.x + glm::cos(shorthand.cur_angle) * shorthand.planet_distance_to_star,
                    position.y,
                    position.z + glm::sin(shorthand.cur_angle) * shorthand.planet_distance_to_star};
            }
        }
    }

    void render(planetgroup& context, glm::vec3 camera_xyz)
    {
        bind(*context.m_base->m_program);
        ubo_planet cur_ubo;
        for (const planet& cur_object : context.m_planets)
        {
            if (glm::distance(camera_xyz, cur_object.base.m_translation) > ZFAR * 0.65)
                continue;
            bind(*context.m_base->m_textures[cur_object.base.m_texture_index], cur_object.base.m_texture_index);
            cur_ubo.transform = get_transform_mat(cur_object.base);
            cur_ubo.inverse_transform = glm::inverse(cur_ubo.transform);
            cur_ubo.texture_index = cur_object.base.m_texture_index;
            cur_ubo.material_ = cur_object.material_;
            memory::update(context.m_ubo, &cur_ubo, sizeof(ubo_planet), 0);
            draw(*context.m_base->m_models[cur_object.base.m_model_index]);
        }
    }
}

}
