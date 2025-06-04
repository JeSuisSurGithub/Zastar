#version 460 core

layout (location = 0) in vec2 in_uv;
layout (location = 0) out vec4 out_rgba;

layout (location = 35) uniform sampler2D main_image;
layout (location = 36) uniform sampler2D bloom;
layout (location = 40) uniform uint screen_tearing_pos;

const float GAMMA = 1.0;
const float EXPOSURE = 0.6;

vec3 vhs_look(sampler2D image, vec2 uv, float chromatic_aberration_amount, float scan_shift, uint scan_height)
{
    const ivec2 resolution = textureSize(image, 0);
    float shift = 0;
    if (gl_FragCoord.y > screen_tearing_pos % resolution.y &&
        gl_FragCoord.y < (screen_tearing_pos + (scan_height * 1/3)) % resolution.y)
    {
        shift = scan_shift * 1.4;
    }
    else if (gl_FragCoord.y >= (screen_tearing_pos + (scan_height * 1/3)) % resolution.y &&
        gl_FragCoord.y < (screen_tearing_pos + (scan_height * 2/3)) % resolution.y)
    {
        shift = scan_shift * 1.2;
    }
    else if (gl_FragCoord.y > (screen_tearing_pos + (scan_height * 2/3)) % resolution.y &&
        gl_FragCoord.y < (screen_tearing_pos + scan_height) % resolution.y)
    {
        shift = scan_shift;
    }
    return (vec3(
        texture(image, uv - (vec2(chromatic_aberration_amount) / textureSize(image, 0)) - vec2(shift, 0)).r,
        texture(image, uv - vec2(shift, 0)).g,
        texture(image, uv + (vec2(chromatic_aberration_amount) / textureSize(image, 0)) - vec2(shift, 0)).b)
    - (mod(gl_FragCoord.y, 2) * 0.20))
    * max(abs(sin((gl_FragCoord.y + screen_tearing_pos) * 0.01)), 0.70);
}

void main()
{
    // vec3 hdr_color = vec3(texture(main_image, in_uv));
    // vec3 bloom_color = vec3(texture(bloom, in_uv));
    vec3 hdr_color = vhs_look(main_image, in_uv, 0.8, 0.008, 8);
    vec3 bloom_color = vhs_look(bloom, in_uv, 0.2, 0.008, 8);
    hdr_color = mix(hdr_color, bloom_color, 0.08);

    vec3 tone_mapped = vec3(1.0) - exp(-hdr_color * EXPOSURE);

    tone_mapped = pow(tone_mapped, vec3(1.0 / GAMMA));
    out_rgba = vec4(tone_mapped, texture(main_image, in_uv).a);
}
