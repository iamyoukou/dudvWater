#version 330

in vec2 tex_coord;

uniform sampler2D tex_subscreen1;

out vec4 outColor;

void main(){
    outColor = texture(tex_subscreen1, tex_coord);
}
