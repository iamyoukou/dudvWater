#version 330

layout(location = 0) in vec3 vtxCoord;
layout(location = 1) in vec2 vtxUv;
layout(location = 2) in vec3 vtxN;

out vec2 uv;
out vec3 worldPos;
out vec3 worldN;

uniform mat4 M, V, P;
// uniform vec4 clipPlane0, clipPlane1;

void main() {
  // NOTE: if tessellation is enabled,
  // the following gl_ClipDistance must be placed in TES
  // gl_ClipDistance[0] = dot(M * vec4(vtxCoord, 1.0), clipPlane0);
  // gl_ClipDistance[1] = dot(M * vec4(vtxCoord, 1.0), clipPlane1);

  uv = vtxUv;

  worldPos = (M * vec4(vtxCoord, 1.0)).xyz;

  worldN = (vec4(vtxN, 1.0) * inverse(M)).xyz;
  worldN = normalize(worldN);
}
