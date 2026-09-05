#ifndef COMMON_GLSL
#define COMMON_GLSL

#define MAX_POINT_LIGHT 256
#define MAX_TEXTURE_COUNT 32

const float RIM_POWER = 2.0;

struct point_light {
    vec3 position;
    vec3 range;
    vec3 color;
};

struct Star {
    mat4 transform;
    mat4 inverse_transform;
    float texture_offset;
    uint texture_index; float _pad0[2];
};

struct Planet {
    mat4 transform;
    mat4 inverse_transform;
    vec3 ambient; float _pad0;
    vec3 diffuse; float _pad1;
    vec3 specular; float _pad2;
    float shininess;  float _pad3[3];
    uint texture_index;float _pad4[3];
};

#endif