#version 460 core

layout(location = 1) in vec3 in_world_xyz;
layout(location = 2) in vec3 in_world_normal;
layout(location = 3) in vec2 in_uv;
layout(location = 4) flat in uint v_instance_id;

layout(location = 0) out vec4 out_rgba;
layout(location = 1) out vec4 out_rgba_bright;

#define MAX_POINT_LIGHT 64
#define MAX_TEXTURE_COUNT 32

layout(location = 0) uniform sampler2D textures[MAX_TEXTURE_COUNT];

const float RIM_POWER = 2.0;

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

struct Planet {
    mat4 transform;
    mat4 inverse_transform;
    vec3 ambient; float _pad0;
    vec3 diffuse; float _pad1;
    vec3 specular; float _pad2;
    float shininess;  float _pad3[3];
    uint texture_index;float _pad4[3];
};

layout (std430, binding = 3) buffer ssbo_planet {
    Planet planets[];
};

vec3 calc_point_light_combined(
    vec3 position,
    vec3 range,
    vec3 color,
    vec3 normal,
    vec3 frag_xyz,
    vec3 view_direction)
{
    vec3 light_direction = normalize(position - frag_xyz);
    float distance = length(position - frag_xyz);
    float attenuation = 1.0 / (range.x + range.y * distance + range.z * (distance * distance));

    vec3 ambient = color * planets[v_instance_id].ambient * attenuation;

    float base_diffuse = max(dot(normal, light_direction), 0.0);
    vec3 halfway_direction = normalize(light_direction + view_direction);
    float base_specular = pow(max(dot(normal, halfway_direction), 0.0), planets[v_instance_id].shininess);
    vec3 phong = ambient +
            base_diffuse * color * planets[v_instance_id].diffuse * attenuation +
            base_specular * color * planets[v_instance_id].specular * attenuation;

    float rim_factor = 1.0 - max(dot(normal, view_direction), 0.0);
    rim_factor = pow(rim_factor, RIM_POWER);
    vec3 rim = ambient + rim_factor * color * attenuation;

    return mix(phong, rim, 0.2);
}

void main()
{
    vec3 normal = normalize(in_world_normal);
    vec3 view_direction = normalize(camera_xyz - in_world_xyz);

    vec3 lighting = vec3(0.0);

    for (uint i = 0; i < current_point_light_count; ++i) {
        lighting += calc_point_light_combined(
            point_lights[i].position,
            point_lights[i].range,
            point_lights[i].color,
            normal,
            in_world_xyz,
            view_direction);
    }

    vec3 texture_color = vec3(0.5) - texture(textures[planets[v_instance_id].texture_index], in_uv).rgb;
    vec4 hdr_color = vec4(lighting * texture_color, 1.0);
    out_rgba = hdr_color;

    float brightness = dot(out_rgba.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        out_rgba_bright = vec4(out_rgba.rgb, 1.0);
    } else {
        out_rgba_bright = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
