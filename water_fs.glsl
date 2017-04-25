#version 330

in vec4 clipSpace;
in vec2 dudv_coord;

uniform sampler2D tex_reflection;
uniform sampler2D tex_refraction;
uniform sampler2D tex_dudv;
uniform float dudv_move;

out vec4 outputColor;

const float alpha = 0.02;
const float beta = 0.02;

void main(){
    vec2 ndc = vec2(clipSpace.x/clipSpace.w, clipSpace.y/clipSpace.w);
    ndc = ndc/2.0 + 0.5;

    vec2 tex_coord_refraction = vec2(ndc.x, ndc.y);
    vec2 tex_coord_reflection = vec2(ndc.x, -ndc.y);

    vec2 distortion1 = texture(tex_dudv, vec2(dudv_coord.x + dudv_move, dudv_coord.y)).rg * 2.0 - 1.0;
    distortion1 *= alpha;
    vec2 distortion2 = texture(tex_dudv, vec2(-dudv_coord.x, dudv_coord.y + dudv_move)).rg * 2.0 - 1.0;
    distortion2 *= beta;
    vec2 distortion = distortion1 + distortion2;

    tex_coord_reflection += distortion;
    tex_coord_reflection.x = clamp(tex_coord_reflection.x, 0.001, 0.999);
    tex_coord_reflection.y = clamp(tex_coord_reflection.y, -0.999, -0.001);

    tex_coord_refraction += distortion;
    tex_coord_refraction = clamp(tex_coord_refraction, 0.001, 0.999);

    vec4 color_reflection = texture(tex_reflection, tex_coord_reflection);
    vec4 color_refraction = texture(tex_refraction, tex_coord_refraction);

    outputColor = mix(color_refraction, color_reflection, 0.5);
    outputColor = mix(outputColor, vec4(0.0, 0.0, 0.1, 1.0), 0.1);
}
