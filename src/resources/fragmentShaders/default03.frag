#version 330 core

in vec2 v_uv;
out vec4 FragColor;

uniform vec2  u_resolution;
uniform float u_time;
uniform float u_fft[64];

void main() {
    float PI = 3.14159265358979;
    vec2 uv = (gl_FragCoord.xy - u_resolution * 0.5) / min(u_resolution.x, u_resolution.y);
    float tx = tan(uv.x);
    float trx = abs(tx - uv.x);

    // Sub-bass enlarges the spiral — values already log-scaled so response feels natural
    float scale = 0.5 + u_fft[0] * 2.5;
    uv /= scale;

    float r     = length(uv);
    float angle = atan(uv.y, uv.x);

    float arms      = 3.0;
    float tightness = 4.5;

    // Mid energy adds a speed burst on top of base rotation
    float speed = uv.y;

    float spiral = mod(
        angle + tightness * log(r + 0.001) - u_time * speed,
        2.0 * PI / arms
    );

    // Low-mid widens the arms slightly
    float width = 0.06 + u_fft[4] * 0.08;
    float band  = smoothstep(width, 0.0, abs(spiral - PI / arms));

    // High freqs add a subtle brightness boost at the outer edge
    float edgeGlow = u_fft[48] * 0.3 * smoothstep(0.3, 0.7, r);

    float fade = smoothstep(0.0, 0.05, u_fft[4] * 1.2) * smoothstep(0.75, 0.4, trx);

    float v = (band * fade) + edgeGlow;
    v = clamp(v, 0.0, 1.0);

    gl_FragColor = vec4(v, v, v, 1.0);
}