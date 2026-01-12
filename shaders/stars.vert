#version 460 core

layout (location = 0) in vec3 in_xyz;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

layout (location = 1) out vec3 out_world_xyz;
layout (location = 2) out vec3 out_world_normal;
layout (location = 3) out vec2 out_uv;
layout (location = 4) flat out uint v_instance_id;

#define MAX_POINT_LIGHT 64
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

struct Star {
    mat4 transform;
    mat4 inverse_transform;
    float texture_offset;
    uint texture_index; float _pad0[2];
};

layout (std430, binding = 2) buffer ssbo_star {
    Star stars[];
};

void main()
{
    vec4 world_xyz = stars[gl_InstanceID].transform * vec4(in_xyz, 1.0);
    gl_Position = projection * view * world_xyz;

    out_world_xyz = vec3(world_xyz);
    out_world_normal = normalize(mat3(transpose(stars[gl_InstanceID].inverse_transform)) * in_normal);
    out_uv = in_uv;
    v_instance_id = gl_InstanceID;
}