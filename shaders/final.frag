#version 460 core

layout (location = 0) in vec2 in_uv;
layout (location = 0) out vec4 out_rgba;

layout (location = 35) uniform sampler2D plain;
layout (location = 36) uniform sampler2D bloom;
layout (location = 40) uniform float time;
layout (location = 41) uniform sampler2D depth;


const float ZFAR = 4e+4;
const float ZNEAR = 1.0;

const int AF_POINT_COUNT = 9;
const vec2 AF_POINTS[9] = vec2[](
    vec2(-0.1, -0.1), vec2( 0.0, -0.1), vec2( 0.1, -0.1),
    vec2(-0.1,  0.0), vec2( 0.0,  0.0), vec2( 0.1,  0.0),
    vec2(-0.1,  0.1), vec2( 0.0,  0.1), vec2( 0.1,  0.1)
);

const float AF_POINT_SCALE = 0.3;
const float AF_ROUNDING = 0.001;
const float AF_FALSE_INFINITY = 1.0 - 1e-1;

const float BLUR_WEIGHTS[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

const float GAMMA = 2.2;
const float EXPOSURE = 1;

const float BLOOM_MIX = 0.1;

const float AF_ZONE_OPACITY = 0.2;

vec3 vhs_look(sampler2D image, vec2 uv, float ca_amount, float tearing_amount, uint tearing_height)
{
    uint screen_tearing_pos = uint(time * 0.25);
    const ivec2 resolution = textureSize(image, 0);
    float shift = 0;
    if (gl_FragCoord.y > screen_tearing_pos % resolution.y &&
        gl_FragCoord.y < (screen_tearing_pos + (tearing_height * 1/3)) % resolution.y)
    {
        shift = tearing_amount * 1.4;
    }
    else if (gl_FragCoord.y >= (screen_tearing_pos + (tearing_height * 1/3)) % resolution.y &&
        gl_FragCoord.y < (screen_tearing_pos + (tearing_height * 2/3)) % resolution.y)
    {
        shift = tearing_amount * 1.2;
    }
    else if (gl_FragCoord.y > (screen_tearing_pos + (tearing_height * 2/3)) % resolution.y &&
        gl_FragCoord.y < (screen_tearing_pos + tearing_height) % resolution.y)
    {
        shift = tearing_amount;
    }
    return (vec3(
        texture(image, uv - (vec2(ca_amount) / textureSize(image, 0)) - vec2(shift, 0)).r,
        texture(image, uv - vec2(shift, 0)).g,
        texture(image, uv + (vec2(ca_amount) / textureSize(image, 0)) - vec2(shift, 0)).b)
    - (mod(gl_FragCoord.y, 2) * 0.20))
    * max(abs(sin((gl_FragCoord.y + screen_tearing_pos) * 0.01)), 0.70);
}

float linear_depth(float depth)
{
    return (2.0 * ZNEAR) / (ZFAR + ZNEAR - depth * (ZFAR - ZNEAR));
}

float compute_focus_depth()
{
    float total = 0.0;
    for (int i = 0; i < AF_POINT_COUNT; ++i) {
        vec2 uv = vec2(0.5) + AF_POINTS[i] * AF_POINT_SCALE;
        total += linear_depth(texture(depth, uv).r);
    }

    float avg = total / float(AF_POINT_COUNT);
    float scale = 1.0 / AF_ROUNDING;
    float focus_depth = round(avg * scale) / scale;
    if (focus_depth > AF_FALSE_INFINITY) {
        return focus_depth = AF_FALSE_INFINITY;
    } else {
        return focus_depth;
    }
}

vec3 star_blur(sampler2D image)
{
    vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, in_uv).rgb * BLUR_WEIGHTS[0];
    for(int i = 1; i < 5; ++i)
    {
        result += texture(image, in_uv + vec2(tex_offset.x * i, 0.0)).rgb * BLUR_WEIGHTS[i];
        result += texture(image, in_uv - vec2(tex_offset.x * i, 0.0)).rgb * BLUR_WEIGHTS[i];
    }
    for(int i = 1; i < 5; ++i)
    {
        result += texture(image, in_uv + vec2(0.0, tex_offset.y * i)).rgb * BLUR_WEIGHTS[i];
        result += texture(image, in_uv - vec2(0.0, tex_offset.y * i)).rgb * BLUR_WEIGHTS[i];
    }
    for(int i = 1; i < 5; ++i)
    {
        result += texture(image, in_uv + vec2(tex_offset.x * i, tex_offset.y * i)).rgb * BLUR_WEIGHTS[i];
        result += texture(image, in_uv - vec2(tex_offset.x * i, tex_offset.y * i)).rgb * BLUR_WEIGHTS[i];
    }
    for(int i = 1; i < 5; ++i)
    {
        result += texture(image, in_uv + vec2(tex_offset.x * i, -tex_offset.y * i)).rgb * BLUR_WEIGHTS[i];
        result += texture(image, in_uv - vec2(tex_offset.x * i, -tex_offset.y * i)).rgb * BLUR_WEIGHTS[i];
    }
    return result / 4.0;
}

vec3 tone_map(vec3 in_rgb)
{
    vec3 tone_mapped = vec3(1.0) - exp(-in_rgb * EXPOSURE);
    tone_mapped = pow(tone_mapped, vec3(1.0 / GAMMA));
    return tone_mapped;
}

void main()
{
    float focus_depth = compute_focus_depth();
    float frag_depth = linear_depth(texture(depth, in_uv).r);
    float depth_diff = clamp(sqrt(sqrt(abs(frag_depth - focus_depth))), 0.0, 1.0);

    vec3 base_color = mix(texture(plain, in_uv).rgb, texture(bloom, in_uv).rgb, BLOOM_MIX);
    vec3 blur_color = mix(star_blur(plain), star_blur(bloom), BLOOM_MIX);
    vec3 dof_color = mix(base_color, blur_color, depth_diff);
    vec3 tone_mapped = tone_map(dof_color);
    // vec3 vhs_color = vhs_look(plain, in_uv, 0.2, 0.008, 8);

    out_rgba = vec4(tone_mapped, texture(plain, in_uv).a);
    if ((abs(vec2(in_uv) - vec2(0.5)).x < AF_POINT_SCALE * 0.1) && (abs(vec2(in_uv) - vec2(0.5)).y < AF_POINT_SCALE * 0.1)) {
        out_rgba = vec4(mix(tone_mapped, vec3(1.0), AF_ZONE_OPACITY), texture(plain, in_uv).a);
    }
    // out_rgba = vec4(mix(tone_mapped, vec3(depth_diff), 0.5), 1.0);
}
