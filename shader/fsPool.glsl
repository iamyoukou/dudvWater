#version 330

in vec3 fragColor;
in vec2 uv;

uniform sampler2D texBase;

out vec4 outColor;

void main(){
    float alpha = 1.0;
    outColor = (1 - alpha)*vec4( fragColor, 1.0 ) + alpha*texture(texBase, uv);
}
