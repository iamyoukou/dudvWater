#version 330
layout( location = 0 ) in vec3 vertex_coord;
layout( location = 1 ) in vec2 in_dudv_coord;

uniform mat4 model_water, view_water, projection_water;
uniform vec3 camera_coord;
uniform vec3 lightPosition_water;

out vec4 clipSpace;
out vec2 dudv_coord;
out vec3 toCamera;
out vec3 fromLightVector;

void main(){
    //projection_water plane
    gl_Position = projection_water * view_water * model_water * vec4( vertex_coord, 1.0 );
    clipSpace = gl_Position;
    dudv_coord = in_dudv_coord;

    vec4 vertex_coord_world = model_water * vec4(vertex_coord, 1.0);
    vec4 camera_coord_world = model_water * vec4(camera_coord, 1.0);
    vec4 lightPosition_water_world = model_water * vec4(lightPosition_water, 1.0);
    toCamera = camera_coord_world.xyz - vertex_coord_world.xyz;
    fromLightVector = vertex_coord_world.xyz - lightPosition_water_world.xyz;
}
