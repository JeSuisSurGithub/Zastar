#include <rendergroups/planetgroup.hpp>

namespace zsl
{

namespace rendergroups
{
    planet::planet(
        rendergroup& group,
        std::shared_ptr<model::model> model,
        std::shared_ptr<texture::texture> texture,
        glm::vec3 position,
        glm::vec3 rotation,
        glm::vec3 scale,
        material material__,
        float distance_to_star_,
        float rev_speed_,
        float orbit_speed_,
        float cur_angle_)
    :
    base(create_object(group, model, texture)),
    material_(material__),
    distance_to_star(distance_to_star_),
    rev_speed(rev_speed_),
    orbit_speed(orbit_speed_),
    cur_angle(cur_angle_)
    {
        base.m_translation = position;
        base.m_euler_angles = rotation;
        base.m_scale = scale;
    }

    planet::~planet() {}

    planetgroup::planetgroup()
    :
    m_base("shaders/planets.vert", "shaders/planets.frag"),
    m_planets()
    {
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

                context.m_planets[planet_count].cur_angle += delta_time * shorthand.orbit_speed;

                rotate(context.m_planets[planet_count].base,
                    glm::vec3(0.0, delta_time * shorthand.rev_speed, 0.0));

                shorthand.base.m_translation = {
                    position.x + glm::cos(shorthand.cur_angle) * shorthand.distance_to_star,
                    position.y,
                    position.z + glm::sin(shorthand.cur_angle) * shorthand.distance_to_star};
            }
        }
    }

    void render(planetgroup& context, glm::vec3 camera_xyz, const glm::vec3& forward, float fov)
    {
        bind(context.m_base.m_shader);
        std::vector<planet_instance> planets;
        for (const planet& cur_object : context.m_planets)
        {
            glm::vec3 direction = glm::normalize(cur_object.base.m_translation - camera_xyz);
            float angle = glm::dot(forward, direction);
            if (angle < (1.f - (fov / 180.f))) {
                continue;
            }

            int texture_index = cur_object.base.m_texture_index;

            bind(*context.m_base.m_textures[texture_index], texture_index);
            planet_instance inst;
            inst.transform = get_transform_mat(cur_object.base);
            inst.inverse_transform = glm::inverse(inst.transform);
            inst.texture_index = texture_index;
            inst.material_ = cur_object.material_;
            planets.push_back(std::move(inst));
        }
        memory::ssbo ssbo_stars(SSBO_BINDINGS::PLANET_DATA, planets.data(),planets.size() * sizeof(planet_instance));
        glBindVertexArray(context.m_base.m_models[context.m_planets[0].base.m_model_index]->m_vao);
        glDrawElementsInstanced(GL_TRIANGLES, context.m_base.m_models[context.m_planets[0].base.m_model_index]->m_indices.size(), GL_UNSIGNED_INT, nullptr, planets.size());
        glBindVertexArray(0);
    }
}

}
