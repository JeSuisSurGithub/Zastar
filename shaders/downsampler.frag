#version 460 core

layout (location = 0) in vec2 in_uv;
layout (location = 0) out vec4 out_rgba;

layout (location = 34) uniform sampler2D in_texture;

void main()
{
    vec2 texel_size = 1.0 / textureSize(in_texture, 0);
    float x = texel_size.x;
    float y = texel_size.y;

    vec3 a = texture(in_texture, vec2(in_uv.x - 2 * x, in_uv.y + 2 * y)).rgb;
    vec3 b = texture(in_texture, vec2(in_uv.x,       in_uv.y + 2 * y)).rgb;
    vec3 c = texture(in_texture, vec2(in_uv.x + 2 * x, in_uv.y + 2 * y)).rgb;

    vec3 d = texture(in_texture, vec2(in_uv.x - 2 * x, in_uv.y)).rgb;
    vec3 e = texture(in_texture, vec2(in_uv.x,       in_uv.y)).rgb;
    vec3 f = texture(in_texture, vec2(in_uv.x + 2 * x, in_uv.y)).rgb;

    vec3 g = texture(in_texture, vec2(in_uv.x - 2 * x, in_uv.y - 2 * y)).rgb;
    vec3 h = texture(in_texture, vec2(in_uv.x,       in_uv.y - 2 * y)).rgb;
    vec3 i = texture(in_texture, vec2(in_uv.x + 2 * x, in_uv.y - 2 * y)).rgb;

    vec3 j = texture(in_texture, vec2(in_uv.x - x, in_uv.y + y)).rgb;
    vec3 k = texture(in_texture, vec2(in_uv.x + x, in_uv.y + y)).rgb;
    vec3 l = texture(in_texture, vec2(in_uv.x - x, in_uv.y - y)).rgb;
    vec3 m = texture(in_texture, vec2(in_uv.x + x, in_uv.y - y)).rgb;

    vec3 downsample = e * 0.125;
    downsample += (a + c + g + i) * 0.03125;
    downsample += (b + d + f + h) * 0.0625;
    downsample += (j + k + l + m) * 0.125;
    out_rgba = vec4(downsample, 1.0);
}