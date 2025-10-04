#ifndef STARGROUP_HPP
#define STARGROUP_HPP

#include "rendergroup.hpp"
#include "ubo.hpp"
#include <memory>

namespace zsl
{
    namespace rendergroups
    {
        typedef struct ubo_star
        {
            alignas(64) glm::mat4 transform;
            alignas(64) glm::mat4 inverse_transform;
            alignas(4) float texture_offset;
            alignas(4) GLuint texture_index;
        }ubo_star;

        typedef struct star
        {
            object base;
            ubo_point_light point_light;
            usz planet_count;
            float texture_offset_count;

            star(
                rendergroup& group,
                std::shared_ptr<model::model> model,
                std::shared_ptr<texture::texture> texture,
                glm::vec3 position,
                glm::vec3 rotation,
                glm::vec3 scale,
                glm::vec3 color,
                glm::vec3 range,
                usz planet_count_);
            ~star();
        }star;

        typedef struct stargroup
        {
            stargroup(const stargroup &) = delete;
            stargroup &operator=(const stargroup &) = delete;
            stargroup(stargroup &&) = delete;
            stargroup &operator=(stargroup &&) = delete;

            rendergroup m_base;

            std::vector<star> m_stars;

            memory::ubo m_ubo;

            stargroup();
            ~stargroup();
        }stargroup;

        void update(stargroup& context, float delta_time);
        void render(stargroup& context, glm::vec3 camera_xyz, const glm::vec3& forward, float fov);

        // According to https://wiki.ogre3d.org/Light+Attenuation+Shortcut
        // Linear = 4.5 / LightRange
        // Quadratic = 75.0 / LightRange^2
        glm::vec3 light_range_constants(float lightrange);
    }
}

#endif /* STARGROUP_HPP */