#include <gen.hpp>

#include <algorithm>

#define SQUARE(X) X * X

namespace zsl
{
    i32 lehmer_randrange(u32& state, i32 min, i32 max)
    {
        state = (std::uint64_t)state * 279470273U % 0xFFFFFFFB;
        return (state % (max - min)) + min;
    }

    float lehmer_randrange_flt(u32& state, float min, float max)
    {
        state = (std::uint64_t)state * 279470273U % 0xFFFFFFFB;
        return (float)state / (float)(UINT32_MAX / (max - min)) + min;
    }

    glm::vec3 lehmer_randrange_vec3(u32& state, glm::vec3 min, glm::vec3 max)
    {
        return glm::vec3(
            lehmer_randrange_flt(state, min.x, max.x),
            lehmer_randrange_flt(state, min.y, max.y),
            lehmer_randrange_flt(state, min.z, max.z));
    }

    void generate(
        rendergroups::stargroup& stars,
        rendergroups::planetgroup& planets,
        u32 seed,
        usz count)
    {
        const glm::vec3 RGB_BIAS = {10.0, 6.0, 8.0};
        usz planet_count = 0;
        std::cout << "Generating " << count << " solar systems..." << std::endl;
        for (usz star_count = 0; star_count < count; star_count++)
        {
            std::cout << "Generating star (" << (star_count + 1) << '/' << count << ")..." << std::endl;
            float star_scale = lehmer_randrange_flt(seed, 20.0, 2000.0);
            glm::vec3 star_position = {
                lehmer_randrange_flt(seed, -(star_scale * SQUARE(count) * 0.10), (star_scale * SQUARE(count) * 0.10)),
                lehmer_randrange_flt(seed, -(star_scale * SQUARE(count) * 0.10), (star_scale * SQUARE(count) * 0.10)),
                lehmer_randrange_flt(seed, -(star_scale * SQUARE(count) * 0.10), (star_scale * SQUARE(count) * 0.10))};
            glm::vec3 star_color = {
                lehmer_randrange_flt(seed, RGB_BIAS.r,  RGB_BIAS.r * 4.0 * (star_scale * 0.1)),
                lehmer_randrange_flt(seed, RGB_BIAS.g,  RGB_BIAS.g * 4.0 * (star_scale * 0.1)),
                lehmer_randrange_flt(seed, RGB_BIAS.b,  RGB_BIAS.b * 4.0 * (star_scale * 0.1))};
            usz star_planet_count = std::clamp<usz>(star_scale * (8.0/1000.0), 2.0, 6.0);
            stars.m_stars.push_back(rendergroups::star(
                *stars.m_base,
                "models/uvs1.obj",
                "textures/star_noise.jpg",
                star_position,
                glm::vec3(0.0),
                glm::vec3(star_scale),
                star_color,
                rendergroups::light_range_constants(8.0 * star_scale),
                star_planet_count));
            std::cout << "Generated star (" << (star_count + 1) << '/' << count << ")" << std::endl;
            std::cout << "Generating " << star_planet_count << " planets..." << std::endl;
            for (usz index = 0; index < star_planet_count; index++, planet_count++)
            {
                std::cout << "\tGenerating planet (" << (index + 1) << '/' << star_planet_count << ")..." << std::endl;
                float planet_scale = lehmer_randrange_flt(seed, star_scale / 40.0, star_scale / 4.0);
                float planet_distance_to_star = lehmer_randrange_flt(seed,
                    ((planet_scale / star_scale) * 80.0 + (planet_scale * 40.0)),
                    ((planet_scale / star_scale) * 700.0 + (planet_scale * 40.0)));
                float planet_revolution_speed = lehmer_randrange_flt(seed,
                    -0.0018,
                    0.0018);
                float planet_orbital_speed = lehmer_randrange_flt(seed,
                    0.0001,
                    0.0005);
                float cur_angle = lehmer_randrange_flt(seed, 0.0, glm::pi<float>());
                glm::vec3 planet_position = {
                    star_position.x + glm::cos(cur_angle) * planet_distance_to_star,
                    star_position.y,
                    star_position.z + glm::sin(cur_angle) * planet_distance_to_star};
                glm::vec3 planet_color = lehmer_randrange_vec3(seed, glm::vec3(0.1), glm::vec3(1.0));
                rendergroups::material material_ = {
                    .material_ambient  = planet_color * lehmer_randrange_flt(seed, 0.3, 1.0),
                    .material_diffuse  = planet_color * lehmer_randrange_flt(seed, 0.3, 1.0),
                    .material_specular = planet_color * lehmer_randrange_flt(seed, 0.3, 1.0),
                    .shininess = lehmer_randrange_flt(seed, 0.3, 1.0)};
                planets.m_planets.push_back(rendergroups::planet(
                    *planets.m_base,
                    "models/uvs1.obj",
                    "textures/venus.png",
                    "textures/planet_noise.png",
                    planet_position,
                    glm::vec3(0.0),
                    glm::vec3(planet_scale),
                    material_,
                    planet_distance_to_star,
                    planet_revolution_speed,
                    planet_orbital_speed,
                    cur_angle));
                std::cout << "\tGenerated planet (" << (index + 1) << '/' << star_planet_count << ")" << std::endl;
            }
            std::cout << "Generated " << star_planet_count << " planets" << std::endl;
        }
        std::cout << "Generated " << count << " solar systems" << std::endl;
        std::cout << "Generated " << planet_count << " planets" << std::endl;
    }
}