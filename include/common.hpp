#ifndef ZSLCOMMON_HPP
#define ZSLCOMMON_HPP

#include <cfloat>

#define ZSL_LOAD_SPIRV true

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

#include <iostream>
#include <string>

namespace zsl
{
    typedef std::int32_t i32;
    typedef std::uint8_t u8;
    typedef std::uint32_t u32;
    typedef signed long long int isz;
    typedef std::size_t usz;

    const std::string WINDOW_NAME{"Zastar"};
    constexpr u32 MAX_TEXTURE_COUNT{32};
    constexpr u32 MAX_POINT_LIGHT{32};
    constexpr float ZFAR{1e+10f};

    typedef enum
    {
        SHARED = 0,
        PLANET = 1,
        STAR = 2,
    }UBO_BINDINGS;

    typedef enum
    {
        TEXT = 0
    }SSBO_BINDINGS;

    typedef enum
    {
        TEXTURE = 0,
        CHARACTER_TEXTURE = 33,
        //CHARACTER_TRANSLATION = 34,
        COMBINE_MAIN_SCENE = 35,
        COMBINE_BLOOM = 36,
        UPSAMPLE_TEXTURE = 37,
        SCREEN_RESOLUTION = 38,
        DOWNSAMPLE_TEXTURE = 39,
        SCREEN_TEARING_SCAN_POS = 40,
    }UNIFORM_LOCATIONS;

    typedef struct vertex
    {
        glm::vec3 xyz;
        glm::vec3 normal;
        glm::vec2 uv;
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