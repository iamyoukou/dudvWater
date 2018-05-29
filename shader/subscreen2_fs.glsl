#version 330

in vec2 tex_coord;

uniform sampler2D tex_subscreen2;

out vec4 outColor;

void main(){
    outColor = texture(tex_subscreen2, tex_coord);
}
