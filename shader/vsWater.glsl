#version 330
layout( location = 0 ) in vec3 vtxCoord;
layout( location = 1 ) in vec2 vtxUv;
layout( location = 2 ) in vec3 vtxN;

uniform mat4 M, V, P;
uniform vec3 camCoord;
uniform vec3 lightPos;

out vec4 clipSpace;
out vec2 dudvCoord;
out vec3 toCamera;
out vec3 fromLightVector;
out vec3 worldPos;
out vec3 worldN;

void main(){
    //projection water plane
    gl_Position = P * V * M * vec4( vtxCoord, 1.0 );
    clipSpace = gl_Position;
    dudvCoord = vtxUv;

    vec4 vtxCoordWorld = M * vec4(vtxCoord, 1.0);
    vec4 camCoordWorld = M * vec4(camCoord, 1.0);
    vec4 lightPosWorld = M * vec4(lightPos, 1.0);
    toCamera = camCoordWorld.xyz - vtxCoordWorld.xyz;
    fromLightVector = vtxCoordWorld.xyz - lightPosWorld.xyz;

    worldPos = (M * vec4(vtxCoord, 1.0)).xyz;

    worldN = normalize((vec4(vtxN, 1.0) * inverse(M)).xyz);
}
