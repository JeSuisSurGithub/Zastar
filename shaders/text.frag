#version 460 core

layout (location = 0) in vec2 in_uv;
layout (location = 0) out vec4 out_rgba;

layout (location = 32) uniform sampler2D characters;

void main()
{
    out_rgba = texture(characters, in_uv);
}