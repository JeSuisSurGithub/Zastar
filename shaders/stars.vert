#version 460 core

layout (location = 0) in vec3 in_xyz;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

layout (location = 1) out vec3 out_world_xyz;
layout (location = 2) out vec3 out_world_normal;
layout (location = 3) out vec2 out_uv;

#define MAX_POINT_LIGHT 32
#define MAX_TEXTURE_COUNT 32

struct point_light {
    vec3 position;
    vec3 range;
    vec3 color;
};

layout (std140, binding = 0) uniform ubo_shared {
    mat4 view;
    mat4 projection;
    vec3 camera_xyz;
    point_light point_lights[MAX_POINT_LIGHT];
    uint current_point_light_count;
};

layout (std140, binding = 2) uniform ubo_star {
    mat4 transform;
    mat4 inverse_transform;
    float texture_offset;
    uint texture_index;
};

void main()
{
    vec4 world_xyz = transform * vec4(in_xyz, 1.0);
    gl_Position = projection * view * world_xyz;

    out_world_xyz = vec3(world_xyz);
    out_world_normal = normalize(mat3(transpose(inverse_transform)) * in_normal);
    out_uv = in_uv;
}