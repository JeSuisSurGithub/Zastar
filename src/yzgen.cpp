#include <yzgen.hpp>

namespace yz
{
    i32 lehmer_randrange(u32& state, i32 min, i32 max)
    {
        state = (std::uint64_t)state * 279470273u % 0xfffffffb;
        return (state % (max - min)) + min;
    }

    float lehmer_randrange_flt(u32& state, float min, float max)
    {
        state = (std::uint64_t)state * 279470273u % 0xfffffffb;
        return (float)state / (float)(UINT32_MAX / (max - min)) + min;
    }

    gen_context::gen_context(
        rendergroups::stargroup& stars,
        rendergroups::planetgroup& planets,
        ubo_point_light* point_lights,
        usz& point_light_count,
        u32 seed,
        usz count)
    {
        std::cout << "Generating " << count << " solar systems..." << std::endl;
        const float SCALE = 1.0;
        const float S_SCALE = SCALE * SCALE;
        const float STAR_SPREAD = 5.0 * count * count;
        const float BASE_POS_RANGE = 50000.0;
        const glm::vec3 RGB_BIAS = {50.0, 30.0, 40.0};
        usz planet_count = 0;
        for (point_light_count = 0; point_light_count < count; point_light_count++)
        {
            std::cout << "Generating star (" << point_light_count + 1 << '/' << count << ")..." << std::endl;
            float scale = lehmer_randrange_flt(seed, SCALE * 10.0, SCALE * 1000.0);
            glm::vec3 position = {
                lehmer_randrange_flt(seed, -(BASE_POS_RANGE / scale) * STAR_SPREAD, (BASE_POS_RANGE / scale) * STAR_SPREAD),
                0.0,
                lehmer_randrange_flt(seed, -(BASE_POS_RANGE / scale) * STAR_SPREAD, (BASE_POS_RANGE / scale) * STAR_SPREAD)};
            glm::vec3 color = {
                lehmer_randrange_flt(seed, 10.0, std::max<float>(RGB_BIAS.r, RGB_BIAS.r * scale / 20.0)),
                lehmer_randrange_flt(seed, 6.0, std::max<float>(RGB_BIAS.g, RGB_BIAS.g * scale / 20.0)),
                lehmer_randrange_flt(seed, 8.0, std::max<float>(RGB_BIAS.b, RGB_BIAS.b * scale / 20.0))};

            add_object(*stars.m_base, "models/uvs1.obj", "textures/unoise.jpg");
            stars.m_base->m_objects[point_light_count].m_translation = position;
            stars.m_base->m_objects[point_light_count].m_scale = glm::vec3(scale, scale, scale);
            point_lights[point_light_count].position = position;
            point_lights[point_light_count].range = rendergroups::light_range_constants(10.0 * scale);
            point_lights[point_light_count].color = color;
            std::cout << "Generated star (" << point_light_count + 1 << '/' << count << ")" << std::endl;

            m_star_planet_count.push_back(std::clamp<usz>(scale * (8.0/1000.0), 2.0, 6.0));
            std::cout << "Generating " << m_star_planet_count[point_light_count] << " planets..." << std::endl;
            for (usz index = 0; index < m_star_planet_count[point_light_count]; index++, planet_count++)
            {
                std::cout << "\tGenerating planet (" << index + 1 << '/' << m_star_planet_count[point_light_count] << ")..." << std::endl;
                float planet_scale = lehmer_randrange_flt(seed, scale / 20.0, scale / 2.0);
                m_planets.push_back(planet_info{
                    .planet_distance_to_star = lehmer_randrange_flt(seed,
                        ((planet_scale / scale) * 80.0 + (planet_scale * 40.0)),
                        ((planet_scale / scale) * 700.0 + (planet_scale * 40.0))),
                    .planet_revolution_speed = lehmer_randrange_flt(seed,
                        -0.0018,
                        0.0018),
                    .planet_orbital_speed = lehmer_randrange_flt(seed,
                        0.0001,
                        0.0005),
                    .cur_angle = lehmer_randrange_flt(seed, 0.0, glm::pi<float>())
                });
                glm::vec3 planet_position = {
                    position.x + glm::cos(m_planets[planet_count].cur_angle) * m_planets[planet_count].planet_distance_to_star,
                    0.0,
                    position.z + glm::sin(m_planets[planet_count].cur_angle) * m_planets[planet_count].planet_distance_to_star};
                glm::vec3 planet_color = {
                    lehmer_randrange_flt(seed, 0.0, 0.5),
                    lehmer_randrange_flt(seed, 0.0, 0.5),
                    lehmer_randrange_flt(seed, 0.0, 0.5)};
                add_object(*planets.m_base, "models/uvs1.obj", "textures/venus.png", "textures/cleanpole.png", planet_color);
                planets.m_base->m_objects[planet_count].m_translation = planet_position;
                planets.m_base->m_objects[planet_count].m_scale
                    = glm::vec3(planet_scale, planet_scale, planet_scale);
                std::cout << "\tGenerated planet (" << index + 1 << '/' << m_star_planet_count[point_light_count] << ")" << std::endl;
            }
            std::cout << "Generated " << m_star_planet_count[point_light_count] << " planets" << std::endl;
        }
        std::cout << "Generated " << count << " solar systems" << std::endl;
    }

    gen_context::~gen_context() {}

    void update(
        gen_context& ctx,
        float delta_time,
        rendergroups::stargroup& stars,
        rendergroups::planetgroup& planets)
    {
        usz planet_count = 0;
        for (usz star_count = 0; star_count < stars.m_base->m_objects.size(); star_count++)
        {
            glm::vec3 position = stars.m_base->m_objects[star_count].m_translation;
            for (usz index = 0; index < ctx.m_star_planet_count[star_count]; index++, planet_count++)
            {
                ctx.m_planets[planet_count].cur_angle += delta_time * ctx.m_planets[planet_count].planet_orbital_speed;
                rotate(planets.m_base->m_objects[planet_count],
                    glm::vec3(0.0, delta_time * ctx.m_planets[planet_count].planet_revolution_speed, 0.0));
                planets.m_base->m_objects[planet_count].m_translation = {
                    position.x + glm::cos(ctx.m_planets[planet_count].cur_angle) * ctx.m_planets[planet_count].planet_distance_to_star,
                    0.0,
                    position.z + glm::sin(ctx.m_planets[planet_count].cur_angle) * ctx.m_planets[planet_count].planet_distance_to_star};
            }
        }
    }
}