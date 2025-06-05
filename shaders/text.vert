#version 460 core

layout (location = 0) in vec2 in_xy;
layout (location = 1) in vec2 in_uv;

layout (location = 0) out vec2 out_uv;

layout (location = 33) uniform vec2 resolution;

layout (std140, binding = 0) buffer ssbo_text {
    vec2 translations[];
};

vec2 to_ogl_space(vec2 xy)
{
    vec2 screen_xy = (xy - (resolution / 2.0)) / (resolution / 2.0);
    return vec2(screen_xy.x, -screen_xy.y);
}

void main()
{
    gl_Position = vec4(to_ogl_space(in_xy + translations[gl_DrawID]), 0.0, 1.0);
    out_uv = in_uv;
}