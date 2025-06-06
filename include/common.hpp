#ifndef ZSLCOMMON_HPP
#define ZSLCOMMON_HPP

#ifdef __unix__
    #define ZSL_LOAD_SPIRV true
#elif defined(_WIN32) || defined(WIN32)
    #define ZSL_LOAD_SPIRV false
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>

#include <cstdint>
#include <string>

namespace zsl
{
    typedef std::int32_t i32;
    typedef std::uint8_t u8;
    typedef std::uint32_t u32;
    typedef std::uint64_t u64;
    typedef signed long long int isz;
    typedef std::size_t usz;

    const std::string WINDOW_NAME{"Zastar"};
    constexpr u32 MAX_TEXTURE_COUNT{32};
    constexpr u32 MAX_POINT_LIGHT{32};
    constexpr float ZFAR{4e+4};
    constexpr float ZNEAR{1.f};

    typedef enum
    {
        SHARED = 0,
        PLANET = 1,
        STAR = 2,
    }UBO_BINDINGS;

    typedef enum
    {
        TEXT = 0,
        VERTEX = 1
    }SSBO_BINDINGS;

    typedef enum
    {
        TEXTURE = 0,
        END_TEXTURE = MAX_TEXTURE_COUNT - 1,
        CHARACTER_TEXTURE = 32,
        SCREEN_RESOLUTION = 33,
        DOWNSAMPLE_TEXTURE = 34,
        UPSAMPLE_TEXTURE = 35,
        PLAIN_IMAGE = 36,
        BLOOM_IMAGE = 37,
        DEPTH_STENCIL_TEXTURE = 38,
        TIME = 39,
        SEED = 40,
        VERTEX_COUNT = 41,
        HEIGHTMAP = 42,
        ITERATIONS = 43,
        NOISE_FREQ = 44,
        GAIN_DECAY = 45,
        FREQ_GAIN  = 46,
    }UNIFORM_LOCATIONS;

    typedef struct vertex
    {
        glm::vec3 xyz;
        float pad0_;
        glm::vec3 normal;
        float pad1_;
        glm::vec2 uv;
        glm::vec2 pad2_;
        bool operator==(const struct vertex& cmp) const
        {
            return (xyz == cmp.xyz && normal == cmp.normal && uv == cmp.uv);
        }
    }vertex;

    typedef struct
    {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 range;
        alignas(16) glm::vec3 color;
    }ubo_point_light;

    typedef struct
    {
        alignas(64) glm::mat4 view;
        alignas(64) glm::mat4 projection;
        alignas(16) glm::vec3 camera_xyz;
        ubo_point_light point_lights[MAX_POINT_LIGHT];
        alignas(4) GLuint point_light_count;
    }ubo_shared;

    constexpr usz ubo_shared_size = 1684;
}

#endif /* ZSLCOMMON_HPP */