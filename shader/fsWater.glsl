#version 330

in vec4 clipSpace;
in vec2 uv;
in vec3 worldPos;
in vec3 worldN;

uniform sampler2D texReflect, texRefract;
uniform sampler2D texDudv, texNormal, texDepthRefr;
uniform samplerCube texSkybox;
uniform float dudvMove;
uniform vec3 lightColor, lightPos;
uniform vec3 eyePoint;

out vec4 fragColor;

const float alpha = 0.2;
const float shineDamper = 300.0;

float fresnelSchlick(float cosTheta, float F0) {
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// https://learnopengl.com/Advanced-OpenGL/Depth-testing
// e.g. can be used to visualize depth value
float linearizeDepth(float d) {
  float near = 0.1;
  float far = 200.0;

  d = d * 2.0 - 1.0;
  d = (2.0 * near * far) / (far + near - d * (far - near));
  d /= far;

  d = min(d, 1.0);

  return d;
}

void main() {
  /* start distorting uv */
  vec2 ndc = vec2(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w);
  ndc = ndc / 2.0 + 0.5;

  vec2 uvRefr = vec2(ndc.x, ndc.y);
  vec2 uvRefl = vec2(ndc.x, -ndc.y);

  // Without alpha, distort will be too huge
  vec2 distort1 = texture(texDudv, vec2(uv.x, uv.y - dudvMove)).rg * 2.0 - 1.0;
  distort1 *= alpha;
  vec2 distort2 = texture(texDudv, vec2(-uv.x, uv.y - dudvMove)).rg * 2.0 - 1.0;
  distort2 *= alpha;
  vec2 distort = distort1 + distort2;

  uvRefl += distort;
  uvRefl.x = clamp(uvRefl.x, 0.001, 0.999);
  uvRefl.y = clamp(uvRefl.y, -0.999, -0.001);

  uvRefr += distort;
  uvRefr = clamp(uvRefr, 0.001, 0.999);
  /* end distorting uv */

  vec3 V = normalize(eyePoint - worldPos);
  float dist = length(eyePoint - worldPos);

  vec3 up = vec3(0, 1, 0);
  vec3 N = texture(texNormal, distort).rgb * 2.0 - 1.0;
  // vec3 L = normalize(lightPos - worldPos);
  vec3 L = normalize(vec3(2, 1, 0));
  vec3 H = normalize(L + V);
  vec3 R = normalize(reflect(L, N));

  // water color
  vec4 deep = vec4(0.003, 0.109, 0.172, 0);
  vec4 sub = vec4(0.054, 0.345, 0.392, 0);

  // reflection and refraction
  vec4 refl = texture(texReflect, uvRefl);

  // better to use a depth value?
  float dFactor = linearizeDepth(texture(texDepthRefr, ndc.xy).r);
  vec4 water = mix(sub, deep, dFactor);
  vec4 refr = mix(texture(texRefract, uvRefr), water, 0.5);

  // air color
  // vec4 air = texture(texSkybox, R);
  vec4 air = vec4(0.1, 0.1, 0.1, 0);

  // reflectivity
  float fresnel = fresnelSchlick(max(dot(V, up), 0.0), 0.02);

  float specFactor = max(dot(H, N), 0.f);
  specFactor = pow(specFactor, shineDamper);
  vec4 specular = vec4(lightColor, 0) * specFactor;

  fragColor = mix(refr, refl, fresnel);
  // fragColor += specular;
}
