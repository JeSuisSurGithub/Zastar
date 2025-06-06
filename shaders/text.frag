#version 460 core

layout (location = 0) in vec2 in_uv;
layout (location = 0) out vec4 out_rgba;

layout (location = 32) uniform sampler2D characters;

vec3 cyan_magenta_ca(sampler2D image, vec2 uv, float amount) {
    return vec3(texture(image, uv + vec2(amount)).r,
        texture(image, uv - vec2(amount)).g,
        (texture(image, uv + vec2(amount)).b +
        texture(image, uv - vec2(amount)).b));
}

void main()
{
    out_rgba = vec4(cyan_magenta_ca(characters, in_uv, 1.0/1024.0), 1.0);
}