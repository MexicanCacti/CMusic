#version 330 core

in vec2 v_uv;
out vec4 FragColor;

uniform vec2 u_resolution;
uniform float u_time;
uniform float u_level;
uniform float u_playing;
uniform float u_fft[64];
uniform float u_hashParams[8];

vec2 rotate(vec2 p, float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return vec2( c * p.x - s * p.y, c * p.y - s * p.x);
}

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;

    uv -= 0.5;
    uv = rotate(uv, u_time);
    uv += 0.5;

    FragColor = vec4(uv.x, uv.y, 0.0, 1.0);
}