#ifndef ZSLGEN_HPP
#define ZSLGEN_HPP

#include "common.hpp"
#include "shader.hpp"
#include "rendergroups/planetgroup.hpp"
#include "rendergroups/stargroup.hpp"

namespace zsl
{
    namespace gen {
        i32 lehmer_randrange(u32& state, i32 min, i32 max);
        float lehmer_randrange_flt(u32& state, float min, float max);
        glm::vec3 lehmer_randrange_vec3(u32& state, glm::vec3 min, glm::vec3 max);

        void generate(
            rendergroups::stargroup& stars,
            rendergroups::planetgroup& planets,
            u32 seed,
            usz count);

        typedef struct noisegen
        {
            noisegen(const noisegen &) = delete;
            noisegen &operator=(const noisegen &) = delete;
            noisegen(noisegen &&) = delete;
            noisegen &operator=(noisegen &&) = delete;

            GLuint m_vao;
            GLuint m_vbo;
            GLuint m_fbo;

            shader::shader m_noise_shader;

            glm::vec2 m_tex_size;
            u32 m_seed;
            float m_noise_freq;
            float m_gain_decay;

            noisegen(const std::string& frag_path, glm::vec2 resolution, u32 seed, float freq, float decay);
            ~noisegen();
        }noisegen;

        std::shared_ptr<texture::texture> generate(noisegen& gen_);
    }
}

#endif /* ZSLGEN_HPP */