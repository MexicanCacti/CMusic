#version 330 core

in vec2 v_uv;
out vec4 FragColor;

uniform vec2 u_resolution;
uniform float u_time;

vec3 rotateAround(vec3 p, float angle, vec2 uv)
{
    return vec3(
        sin(uv.x * angle * u_time) + cos(uv.y * angle) * p.x * floor(max(p.z, 0.0)),
        cos(p.y * angle * uv.y) - sin(uv.y * p.x * u_time),
        tan(p.z * angle) * step(u_time, angle * cos(u_time * uv.x) + uv.y)
    );
}

vec3 spots(vec3 p, vec2 uv)
{
    float denom = max(abs(sin(p.x)), 0.001);

    return vec3(
        p.x * uv.y + cos(u_time * 2.0) - 0.2,
        p.y * uv.x * uv.y / denom,
        p.z * uv.x * uv.y * p.y
    );
}

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;

    float tx = tan(uv.x);
    float r = abs(tx - uv.x);

    vec3 col = vec3(
        abs(r) + uv.y,
        abs(0.2 * r) - uv.y,
        pow(max(uv.x, 0.0001), max(uv.y, 0.0001))
    );

    col = rotateAround(col, sign(r * max(u_time, 0.0001)), uv);
    col = rotateAround(col, pow(max(u_time, 0.0001), max(uv.y, 0.0001)), uv);
    col = spots(col, uv);

    col /= max(exp(r) / 2.0, 0.001);

    FragColor = vec4(col, 1.0);
}