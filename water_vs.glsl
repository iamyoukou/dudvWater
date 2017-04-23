#version 330
layout( location = 0 ) in vec3 vertex_coord;

uniform mat4 model_water, view_water, projection_water;

void main(){
    //projection_water plane
    gl_Position = projection_water * view_water * model_water * vec4( vertex_coord, 1.0 );
}
