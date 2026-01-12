#ifndef PLANETGROUP_HPP
#define PLANETGROUP_HPP

#include "common.hpp"
#include "rendergroup.hpp"
#include "ubo.hpp"
#include "rendergroups/stargroup.hpp"

namespace zsl
{
    namespace rendergroups
    {
        typedef struct material
        {
            glm::vec3 ambient; float _pad0;
            glm::vec3 diffuse; float _pad1;
            glm::vec3 specular; float _pad2;
            float shininess; float _pad3[3];
        }material;

        typedef struct planet_instance
        {
            glm::mat4 transform;
            glm::mat4 inverse_transform;
            material material_;
            GLuint texture_index;
            float _pad0[3];
        }planet_instance;

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
                std::shared_ptr<model::model> model,
                std::shared_ptr<texture::texture> texture,
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

            rendergroup m_base;

            std::vector<planet> m_planets;

            planetgroup();
            ~planetgroup();
        }planetgroup;

        void update(planetgroup& context, float delta_time, const std::vector<star>& stars);
        void render(planetgroup& context, glm::vec3 camera_xyz, const glm::vec3& forward, float fov);
    }
}

#endif /* PLANETGROUP_HPP */