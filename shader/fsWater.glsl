#version 330

in vec4 clipSpace;
in vec2 dudvCoord;
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

    vec2 texCoordRefract = vec2(ndc.x, ndc.y);
    vec2 texCoordReflect = vec2(ndc.x, -ndc.y);

    // Without alpha, distortion will be too huge
    vec2 distortion1 = texture(texDudv, vec2(dudvCoord.x + dudvMove, dudvCoord.y)).rg * 2.0 - 1.0;
    distortion1 *= alpha;
    vec2 distortion2 = texture(texDudv, vec2(-dudvCoord.x, dudvCoord.y + dudvMove)).rg * 2.0 - 1.0;
    distortion2 *= alpha;
    vec2 distortion = distortion1 + distortion2;

    texCoordReflect += distortion;
    texCoordReflect.x = clamp(texCoordReflect.x, 0.001, 0.999);
    texCoordReflect.y = clamp(texCoordReflect.y, -0.999, -0.001);

    texCoordRefract += distortion;
    texCoordRefract = clamp(texCoordRefract, 0.001, 0.999);

    vec4 color_reflection = texture(texReflect, texCoordReflect);
    vec4 color_refraction = texture(texRefract, texCoordRefract);

    vec4 normalMapColor = texture(texNormal, distortion);
    vec3 normal = vec3(normalMapColor.r*2.0-1.0, normalMapColor.b*2.0-1.0, normalMapColor.g*2.0-1.0);

    vec3 viewVector = normalize(toCamera);
    float refractiveFactor = dot(viewVector, vec3(0, 1, 0));
    refractiveFactor = pow(refractiveFactor, 3.0);

    vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shineDamper);
    vec3 specularHighlight = lightColor * specular * reflectivity;

    // outputColor = mix(color_reflection, color_refraction, refractiveFactor);

    // temporarily
    outputColor = vec4(0.66, 0.71, 0.84, 0);
    outputColor = mix(outputColor, vec4(0.0, 0.0, 0.1, 1.0) + vec4(specularHighlight, 0), 0.1);
}
