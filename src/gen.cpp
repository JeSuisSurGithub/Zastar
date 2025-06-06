#include <gen.hpp>
#include <framebuffer.hpp>

#include <algorithm>
#include <iostream>

#include <cstdint>
#include <memory>

#define SQUARE(X) X * X

namespace zsl
{
namespace gen
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
        std::shared_ptr<model::model> star_model = std::make_shared<model::model>("models/uvs_flat.obj");
        noisegen star_tex_gen{"shaders/fbm.frag", glm::vec2(512.f, 512.f), seed, 5, 4.0, 0.5, 2.0};
        std::shared_ptr<texture::texture> star_texture = generate(star_tex_gen);

        noisegen hmap_tex_gen{"shaders/fbm.frag", glm::vec2(1024.f, 1024.f), seed, 5, 16.0, 0.7, 2.0};
        std::shared_ptr<texture::texture> height_map = generate(hmap_tex_gen);
        std::shared_ptr<model::model> planet_model = std::make_shared<model::model>("models/uvs_flat2.obj", height_map);
        // std::shared_ptr<texture::texture> planet_texture = std::make_shared<texture::texture>("textures/patatouille.png");

        const glm::vec3 RGB_BIAS = {2.0, 1.0, 1.5};
        const float pi = glm::pi<float>();
        usz planet_count = 0;
        std::cout << "Generating " << count << " solar systems..." << std::endl;
        for (usz star_count = 0; star_count < count; star_count++)
        {
            std::cout << "Generating star (" << (star_count + 1) << '/' << count << ")..." << std::endl;
            float star_scale = lehmer_randrange_flt(seed, 2.0, 200.0);

            float max_star_pos_range = star_scale * SQUARE(count) * 0.1;

            glm::vec3 star_position = {
                lehmer_randrange_flt(seed, -max_star_pos_range, max_star_pos_range),
                lehmer_randrange_flt(seed, -max_star_pos_range, max_star_pos_range),
                lehmer_randrange_flt(seed, -max_star_pos_range, max_star_pos_range)};

            glm::vec3 star_color = {
                lehmer_randrange_flt(seed, RGB_BIAS.r,  RGB_BIAS.r * 4.0 * star_scale),
                lehmer_randrange_flt(seed, RGB_BIAS.g,  RGB_BIAS.g * 4.0 * star_scale),
                lehmer_randrange_flt(seed, RGB_BIAS.b,  RGB_BIAS.b * 4.0 * star_scale)};

            usz star_planet_count = std::clamp<usz>(star_scale * (8.0/64.0), 2.0, 8.0);

            stars.m_stars.push_back(rendergroups::star(
                stars.m_base,
                star_model,
                star_texture,
                star_position,
                lehmer_randrange_vec3(seed, glm::vec3(-pi), glm::vec3(pi)),
                glm::vec3(star_scale),
                star_color,
                rendergroups::light_range_constants(4.0 * star_scale),
                star_planet_count));

            std::cout << "Generated star (" << (star_count + 1) << '/' << count << ")" << std::endl;
            std::cout << "Generating " << star_planet_count << " planets..." << std::endl;

            for (usz index = 0; index < star_planet_count; index++, planet_count++)
            {
                std::cout << "\tGenerating planet (" << (index + 1) << '/' << star_planet_count << ")..." << std::endl;

                float planet_scale = lehmer_randrange_flt(seed, star_scale / 32.0, star_scale / 8.0);

                float distance_to_star = lehmer_randrange_flt(seed,
                    ((planet_scale / star_scale) * 64.0 + (planet_scale * 64.0)),
                    ((planet_scale / star_scale) * 128.0 + (planet_scale * 128.0)));

                float rev_speed = lehmer_randrange_flt(seed,
                    -1.f/1024,
                    1.f/1024);

                float orbit_speed = lehmer_randrange_flt(seed,
                    1.f/8192,
                    1.f/2048);

                float cur_angle = lehmer_randrange_flt(seed, 0.0, pi);

                glm::vec3 planet_position = {
                    star_position.x + glm::cos(cur_angle) * distance_to_star,
                    star_position.y,
                    star_position.z + glm::sin(cur_angle) * distance_to_star};

                rendergroups::material material_ = {
                    .material_ambient  = lehmer_randrange_vec3(seed, glm::vec3(0.1, 0.1, 0.0), glm::vec3(0.4, 0.4, 0.6)),
                    .material_diffuse  = glm::vec3(lehmer_randrange_flt(seed, 0.1, 0.4)),
                    .material_specular = glm::vec3(lehmer_randrange_flt(seed, 1.0, 4.0)),
                    .shininess = lehmer_randrange_flt(seed, 2.0, 4.0)};

                planets.m_planets.push_back(rendergroups::planet(
                    planets.m_base,
                    planet_model,
                    height_map,
                    planet_position,
                    lehmer_randrange_vec3(seed, glm::vec3(-pi / 4), glm::vec3(pi / 4)),
                    glm::vec3(planet_scale),
                    material_,
                    distance_to_star,
                    rev_speed,
                    orbit_speed,
                    cur_angle));

                std::cout << "\tGenerated planet (" << (index + 1) << '/' << star_planet_count << ")" << std::endl;
            }
            std::cout << "Generated " << star_planet_count << " planets" << std::endl;
        }
        std::cout << "Generated " << count << " solar systems" << std::endl;
        std::cout << "Generated " << planet_count << " planets" << std::endl;
    }

    noisegen::noisegen(const std::string& frag_path, glm::vec2 resolution, u32 seed, int iteration, float freq, float decay, float gain)
    :
    m_vao(0),
    m_vbo(0),
    m_fbo(0),
    m_noise_shader("shaders/quad.vert", frag_path, ZSL_LOAD_SPIRV),
    m_tex_size(resolution),
    m_seed(seed),
    m_iterations(iteration),
    m_noise_freq(freq),
    m_gain_decay(decay),
    m_lacunarity(gain)
    {
        glCreateBuffers(1, &m_vbo);
        glNamedBufferStorage(m_vbo, sizeof(framebuffer::FULL_QUAD), &framebuffer::FULL_QUAD, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        glCreateVertexArrays(1, &m_vao);
        glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, (4 * sizeof(float)));
        glEnableVertexArrayAttrib(m_vao, 0);
        glEnableVertexArrayAttrib(m_vao, 1);
        glVertexArrayAttribFormat(m_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE, (2 * sizeof(float)));
        glVertexArrayAttribBinding(m_vao, 0, 0);
        glVertexArrayAttribBinding(m_vao, 1, 0);

        glCreateFramebuffers(1, &m_fbo);
    }

    noisegen::~noisegen()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteFramebuffers(1, &m_fbo);
    }

    std::shared_ptr<texture::texture> generate(noisegen& gen_)
    {
        std::shared_ptr<texture::texture> noise =
            std::make_shared<texture::texture>(gen_.m_tex_size.x, gen_.m_tex_size.y, GL_LINEAR, GL_REPEAT, GL_RGBA8);

        bind_to_framebuffer(*noise, gen_.m_fbo, GL_COLOR_ATTACHMENT0);

        GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
        glNamedFramebufferDrawBuffers(gen_.m_fbo, 1, attachments);

        GLenum status = glCheckNamedFramebufferStatus(gen_.m_fbo, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Off screen framebuffer not complete");

        shader::update_float(gen_.m_noise_shader, UNIFORM_LOCATIONS::SEED, lehmer_randrange_flt(gen_.m_seed, 0.0, 1.0));
        shader::update_int(gen_.m_noise_shader, UNIFORM_LOCATIONS::ITERATIONS, gen_.m_iterations);
        shader::update_float(gen_.m_noise_shader, UNIFORM_LOCATIONS::NOISE_FREQ, gen_.m_noise_freq);
        shader::update_float(gen_.m_noise_shader, UNIFORM_LOCATIONS::GAIN_DECAY, gen_.m_gain_decay);
        shader::update_float(gen_.m_noise_shader, UNIFORM_LOCATIONS::FREQ_GAIN, gen_.m_lacunarity);
        bind(gen_.m_noise_shader);

        glBindFramebuffer(GL_FRAMEBUFFER, gen_.m_fbo);
        glViewport(0, 0, gen_.m_tex_size.x, gen_.m_tex_size.y);

        glBindVertexArray(gen_.m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);

        return noise;
    }
}
}