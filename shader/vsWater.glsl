#version 330
layout( location = 0 ) in vec3 vtxCoord;
layout( location = 1 ) in vec2 vtxUv;

uniform mat4 M, V, P;
uniform vec3 camCoord;
uniform vec3 lightPos;

out vec4 clipSpace;
out vec2 dudv_coord;
out vec3 toCamera;
out vec3 fromLightVector;

void main(){
    //projection water plane
    gl_Position = P * V * M * vec4( vtxCoord, 1.0 );
    clipSpace = gl_Position;
    dudv_coord = vtxUv;

    vec4 vertex_coord_world = M * vec4(vtxCoord, 1.0);
    vec4 camera_coord_world = M * vec4(camCoord, 1.0);
    vec4 lightPosition_water_world = M * vec4(lightPos, 1.0);
    toCamera = camera_coord_world.xyz - vertex_coord_world.xyz;
    fromLightVector = vertex_coord_world.xyz - lightPosition_water_world.xyz;
}
