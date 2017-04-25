#version 330
layout( location = 0 ) in vec3 vertex_coord;
layout( location = 1 ) in vec2 in_dudv_coord;

uniform mat4 model_water, view_water, projection_water;

out vec4 clipSpace;
out vec2 dudv_coord;

void main(){
    //projection_water plane
    gl_Position = projection_water * view_water * model_water * vec4( vertex_coord, 1.0 );
    clipSpace = gl_Position;
    dudv_coord = in_dudv_coord;
}
