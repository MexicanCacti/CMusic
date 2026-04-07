#version 330 core

in vec2 v_uv;
out vec4 FragColor;

uniform vec2  u_resolution;
uniform float u_time;
uniform float u_fft[64];

float getFFTValue(float pos)
{
    int index = int(floor(clamp(pos, 0.0, 0.9999) * 64.0));
    return u_fft[index];
}

float rotate(float val, vec2 uv)
{
    float rotVal = uv.x / uv.y;
    float PI = 3.14159265358979;
    return val * (rotVal * PI) * log(length(uv));
}

void main() {
    float PI = 3.14159265358979;
    vec2 uv = (gl_FragCoord.xy - u_resolution * 0.5) / min(u_resolution.x, u_resolution.y);
    float r = length(uv);
    float angle = atan(uv.y, uv.x);

    float middleTarget = tan(r * u_time) * max(u_fft[0], 0.5);
    float blueTarget = middleTarget + 0.5;
    float blueOffset = log(blueTarget * max(u_fft[48], 3.5) / PI);
    float clampedTime = clamp(u_time, 0.01, 0.1);


    gl_FragColor = sin(vec4((middleTarget), tan(rotate(clampedTime, uv)) * u_fft[0], blueOffset, 1.0));
}