#version 460 core

layout (location = 0) in vec2 in_uv;
layout (location = 0) out vec4 out_rgba;

layout (location = 35) uniform sampler2D in_texture;

const float filter_radius = 1.0;

void main()
{
    float x = filter_radius;
    float y = filter_radius;

    vec3 a = texture(in_texture, vec2(in_uv.x - x, in_uv.y + y)).rgb;
    vec3 b = texture(in_texture, vec2(in_uv.x,     in_uv.y + y)).rgb;
    vec3 c = texture(in_texture, vec2(in_uv.x + x, in_uv.y + y)).rgb;

    vec3 d = texture(in_texture, vec2(in_uv.x - x, in_uv.y)).rgb;
    vec3 e = texture(in_texture, vec2(in_uv.x,     in_uv.y)).rgb;
    vec3 f = texture(in_texture, vec2(in_uv.x + x, in_uv.y)).rgb;

    vec3 g = texture(in_texture, vec2(in_uv.x - x, in_uv.y - y)).rgb;
    vec3 h = texture(in_texture, vec2(in_uv.x,     in_uv.y - y)).rgb;
    vec3 i = texture(in_texture, vec2(in_uv.x + x, in_uv.y - y)).rgb;

    vec3 upsample = e * 4.0;
    upsample += (b + d + f + h) * 2.0;
    upsample += (a + c + g + i);
    upsample *= 1.0 / 16.0;
    out_rgba = vec4(upsample, 1.0);
}