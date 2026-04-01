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
        sin(uv.x * angle * u_time) + cos(uv.y * angle) * p.x * floor(max(getFFTValue(p.z), 0.0)),
        cos(p.y * angle * uv.y) - sin(uv.y * p.x * u_time),
        log(p.z * angle) * mix(u_time, angle * cos(u_time * uv.x) + uv.y, getFFTValue(p.x))
    );
}

vec3 spots(vec3 p, vec2 uv)
{
    float denom = max(abs(getFFTValue(uv.y)), 0.001);

    return vec3(
        0.01,
        p.y * uv.x * uv.y / denom,
        p.z * uv.x * uv.y * sampleFFT(uv.x * 0.2)
    );
}

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;
    int xSample =  getSampleIndex(uv.x);
    int ySample =  getSampleIndex(uv.y);
    float tx = tan(uv.x);
    float r = abs(tx - uv.x);

    vec3 col = vec3(
        sin(r + u_fft[ySample]) + uv.y,
        sin(uv.x * r) - uv.y,
        cos(max(uv.x, 0.0001), max(uv.y, 0.0001))
    );

    col = rotateAround(col, sign(r * max(u_time, 0.0001)), uv);
    col = rotateAround(col, pow(max(u_time * r * u_fft[xSample], 0.0001), max(uv.y, 0.0001)), uv);
    col = spots(col, uv);

    col /= max(exp(r) / 2.0, 0.001);

    // FFT debug overlay
    float fft = sampleFFT(uv.x);

    // bar fill
    //float bar = log(fft);

    // bright line at top of each bar
    //float line = 1.0 - smoothstep(0.0, 0.015, abs(uv.y - fft * (u_time)));

    //vec3 barColor = mix(
    //    vec3(0.1, 0.8, 0.2),
    //    vec3(1.0, 0.2, 0.1),
    //    fft
    //);

    // overlay spectrum bars
    //col += bar * barColor * 0.6;

    // overlay bright FFT trace
    //col += line * vec3(1.0, 1.0, 1.0) * 1.0;


    // overall pulse from average FFT energy
    //col *= 0.8 + fft * bar;

    FragColor = vec4(col, 1.0);
}