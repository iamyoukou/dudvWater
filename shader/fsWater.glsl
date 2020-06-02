#version 330

in vec4 clipSpace;
in vec2 dudv_coord;
in vec3 toCamera;
in vec3 fromLightVector;

uniform sampler2D texReflect;
uniform sampler2D texRefract;
uniform sampler2D texDudv;
uniform sampler2D texNormal;
uniform sampler2D tex_depth;
uniform float dudvMove;
uniform vec3 lightColor;

out vec4 outputColor;

const float alpha = 0.02;
const float shineDamper = 10.0;
const float reflectivity = 0.6;

void main(){
    vec2 ndc = vec2(clipSpace.x/clipSpace.w, clipSpace.y/clipSpace.w);
    ndc = ndc/2.0 + 0.5;

    vec2 tex_coord_refraction = vec2(ndc.x, ndc.y);
    vec2 tex_coord_reflection = vec2(ndc.x, -ndc.y);

    // Without alpha, distortion will be too huge
    vec2 distortion1 = texture(texDudv, vec2(dudv_coord.x + dudvMove, dudv_coord.y)).rg * 2.0 - 1.0;
    distortion1 *= alpha;
    vec2 distortion2 = texture(texDudv, vec2(-dudv_coord.x, dudv_coord.y + dudvMove)).rg * 2.0 - 1.0;
    distortion2 *= alpha;
    vec2 distortion = distortion1 + distortion2;

    tex_coord_reflection += distortion;
    tex_coord_reflection.x = clamp(tex_coord_reflection.x, 0.001, 0.999);
    tex_coord_reflection.y = clamp(tex_coord_reflection.y, -0.999, -0.001);

    tex_coord_refraction += distortion;
    tex_coord_refraction = clamp(tex_coord_refraction, 0.001, 0.999);

    vec4 color_reflection = texture(texReflect, tex_coord_reflection);
    vec4 color_refraction = texture(texRefract, tex_coord_refraction);

    vec4 normalMapColor = texture(texNormal, distortion);
    vec3 normal = vec3(normalMapColor.r*2.0-1.0, normalMapColor.b*2.0-1.0, normalMapColor.g*2.0-1.0);

    vec3 view_vector = normalize(toCamera);
    float refractive_factor = dot(view_vector, vec3(0, 1, 0));
    refractive_factor = pow(refractive_factor, 3.0);

    vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
    float specular = max(dot(reflectedLight, view_vector), 0.0);
    specular = pow(specular, shineDamper);
    vec3 specularHighlight = lightColor * specular * reflectivity;

    outputColor = mix(color_reflection, color_refraction, refractive_factor);
    outputColor = mix(outputColor, vec4(0.0, 0.0, 0.1, 1.0) + vec4(specularHighlight, 0), 0.1);
}
