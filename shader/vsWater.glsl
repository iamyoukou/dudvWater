#version 330
layout(location = 0) in vec3 vtxCoord;
layout(location = 1) in vec2 vtxUv;
layout(location = 2) in vec3 vtxN;

uniform mat4 M, V, P;

out vec4 clipSpace;
out vec2 uv;
out vec3 worldPos;
out vec3 worldN;

void main()
{
    gl_Position = P * V * M * vec4(vtxCoord, 1.0);
    clipSpace = gl_Position;
    uv = vtxUv;
    worldPos = (M * vec4(vtxCoord, 1.0)).xyz;
    worldN = normalize((vec4(vtxN, 1.0) * inverse(M)).xyz);
}
