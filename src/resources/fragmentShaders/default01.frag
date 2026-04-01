#version 330 core

in vec2 v_uv;
out vec4 FragColor;

uniform vec2 u_resolution;
uniform float u_time;
uniform float u_fft[64];

float sampleFFT(float x)
{
    float pos = clamp(x, 0.0, 0.9999) * 64.0;
    int i = int(floor(pos));
    int j = min(i + 1, 63);
    float t = fract(pos);
    return mix(u_fft[i], u_fft[j], t);
}

int getSampleIndex(float pos)
{
    return int(floor(clamp(pos, 0.0, 0.9999) * 64.0));
}

float getFFTValue(float pos)
{
    int index = int(floor(clamp(pos, 0.0, 0.9999) * 64.0));
    return u_fft[index];
}

vec3 rotateAround(vec3 p, float angle, vec2 uv)
{
    return vec3(
        sin(uv.x * angle * u_time) + cos(uv.y * angle) * p.x * floor(max(p.z, 0.0)),
        cos(p.y * angle * uv.y) - sin(uv.y * p.x * u_time),
        tan(p.z * angle) * step(u_time, angle * getFFTValue(u_time * angle) + uv.y)
    );
}

vec3 spots(vec3 p, vec2 uv)
{
    float denom = max(abs(p.x), 0.001);

    return vec3(
        0.0,
        p.y * uv.x * uv.y / denom,
        getFFTValue(p.z) * uv.x * uv.y
    );
}

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;

    float tx = tan(uv.x);
    float r = abs(tx - uv.x);

    vec3 col = vec3(
        0.0,
        abs(getFFTValue(sin(uv.y) * r) - uv.y),
        pow(max(uv.x, 0.0001), max(uv.y, 0.0001))
    );

    col = rotateAround(col, fract(r * min(getFFTValue(u_time), getFFTValue(r))), uv);
    col = rotateAround(col, pow(max(u_time, getFFTValue(r)), max(uv.y, 0.0001)), uv);
    col = spots(col, uv);

    col *= max(getFFTValue(r) * r, 0.5);

    FragColor = vec4(col, 1.0);
}