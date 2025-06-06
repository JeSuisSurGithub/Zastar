#version 460 core

layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 out_rgba;
layout(location = 40) uniform float seed;

layout(location = 43) uniform int iterations;
layout(location = 44) uniform float noise_freq;
layout(location = 45) uniform float gain_decay;
layout(location = 46) uniform float freq_gain;

// Random Normalize Gradients
vec2 gradient(vec2 p)
{
    float angle = fract(sin(dot(p, vec2(127.1 + seed, 311.7 + seed))) * (43758.5453 + seed)) * 6.2831853;
    return vec2(cos(angle), sin(angle));
}

float grad_noise(vec2 uv, float period)
{
    uv *= period;

    vec2 i = floor(uv);
    vec2 f = fract(uv);

    // Wrapping
    vec2 i00 = mod(i + vec2(0.0, 0.0), period);
    vec2 i10 = mod(i + vec2(1.0, 0.0), period);
    vec2 i01 = mod(i + vec2(0.0, 1.0), period);
    vec2 i11 = mod(i + vec2(1.0, 1.0), period);

    vec2 g00 = gradient(i00);
    vec2 g10 = gradient(i10);
    vec2 g01 = gradient(i01);
    vec2 g11 = gradient(i11);

    vec2 d00 = f - vec2(0.0, 0.0);
    vec2 d10 = f - vec2(1.0, 0.0);
    vec2 d01 = f - vec2(0.0, 1.0);
    vec2 d11 = f - vec2(1.0, 1.0);

    float v00 = dot(g00, d00);
    float v10 = dot(g10, d10);
    float v01 = dot(g01, d01);
    float v11 = dot(g11, d11);

    // Smoothing
    vec2 u = f * f * (3.0 - 2.0 * f);

    float x1 = mix(v00, v10, u.x);
    float x2 = mix(v01, v11, u.x);
    return mix(x1, x2, u.y);
}

float fbm(vec2 uv, int octaves, float base_freq, float gain, float lacunarity)
{
    float sum = 0.0;
    float amp = 0.5;
    float freq = base_freq;
    for (int i = 0; i < octaves; ++i) {
        sum += grad_noise(uv, freq) * amp;
        freq *= lacunarity;
        amp *= gain;
    }
    return sum;
}

float positive(float noise) {
    return noise * 0.5 + 0.5;
}

float compress_mid_range(float noise) {
    return pow(positive(noise), 4.0);
}

float veins(float noise) {
    return abs(noise);
}

float double_fbm(vec2 uv, int octaves, float base_freq, float gain, float lacunarity) {
    float noise = fbm(in_uv, octaves / 2, noise_freq / 2.0, gain_decay, lacunarity);
    float pnoise = positive(noise);

    return fbm(vec2(pnoise), octaves, noise_freq, gain_decay, lacunarity);
}

void main()
{
    float noise = fbm(in_uv, iterations, noise_freq, gain_decay, freq_gain);
    out_rgba = vec4(vec3(compress_mid_range(noise)), 1.0);
}