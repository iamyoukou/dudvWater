#version 330

in vec3 fragmentColor;
in vec2 tex_coord;

uniform sampler2D tex;

out vec4 outputColor;

void main(){
    float alpha = 0.9;
    outputColor = (1 - alpha)*vec4( fragmentColor, 1.0 ) + alpha*texture(tex, tex_coord);
}
