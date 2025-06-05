#ifndef PLANETGROUP_HPP
#define PLANETGROUP_HPP

#include "rendergroup.hpp"
#include "ubo.hpp"
#include "rendergroups/stargroup.hpp"

#include <memory>

namespace zsl
{
    namespace rendergroups
    {
        typedef struct material
        {
            alignas(16) glm::vec3 material_ambient;
            alignas(16) glm::vec3 material_diffuse;
            alignas(16) glm::vec3 material_specular;
            float shininess;
        }material;

        typedef struct ubo_planet
        {
            alignas(64) glm::mat4 transform;
            alignas(64) glm::mat4 inverse_transform;
            alignas(4) GLuint texture_index;
            material material_;
        }ubo_planet;

        typedef struct planet
        {
            object base;
            material material_;
            float distance_to_star;
            float rev_speed;
            float orbit_speed;
            float cur_angle;

            planet(
                rendergroup& group,
                const std::string& model_path,
                const std::string& texture_path,
                const std::string& height_map_path,
                glm::vec3 position,
                glm::vec3 rotation,
                glm::vec3 scale,
                material material__,
                float distance_to_star,
                float rev_speed,
                float orbit_speed,
                float cur_angle);
            ~planet();
        }planet;

        typedef struct planetgroup
        {
            planetgroup(const planetgroup &) = delete;
            planetgroup &operator=(const planetgroup &) = delete;
            planetgroup(planetgroup &&) = delete;
            planetgroup &operator=(planetgroup &&) = delete;

            std::unique_ptr<rendergroup> m_base;

            std::vector<planet> m_planets;

            memory::ubo m_ubo;

            planetgroup();
            ~planetgroup();
        }planetgroup;

        void update(planetgroup& context, float delta_time, const std::vector<star>& stars);
        void render(planetgroup& context, glm::vec3 camera_xyz);
    }
}

#endif /* PLANETGROUP_HPP */