#include "common.h"
#include "water.h"

Water::Water() {
  initShader();
  initBuffer();
  initTexture();
  initUniform();
}

Water::~Water() {}

void Water::draw(mat4 M, mat4 V, mat4 P, vec3 eyePoint, vec3 lightColor,
                 vec3 lightPosition, float dudvMove) {
  glUseProgram(shader);

  dudvMove += 0.0005f; // speed
  dudvMove = fmod(dudvMove, 1.0f);

  glUniform1f(uniDudvMove, dudvMove);
  glUniform3fv(uniCamCoord, 1, value_ptr(eyePoint));

  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  glUniform3fv(uniLightPos, 1, value_ptr(lightPosition));

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Water::initBuffer() {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vtxs), vtxs, GL_STATIC_DRAW);
  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  // uv
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
                        (GLvoid *)(sizeof(GLfloat) * 6 * 3));
  glEnableVertexAttribArray(1);
}

void Water::initShader() {
  shader = buildShader("./shader/vsPool.glsl", "./shader/fsPool.glsl");
}

void Water::initTexture() {
  // water dudv map
  setTexture(tboDudv, 11, "./image/dudv2.png", FIF_PNG);

  // water normal map
  setTexture(tboNormal, 12, "./image/normalMap2.png", FIF_PNG);
}

void Water::initUniform() {
  glUseProgram(shader);

  // transform
  uniM = myGetUniformLocation(shader, "M");
  uniV = myGetUniformLocation(shader, "V");
  uniP = myGetUniformLocation(shader, "P");

  // glUniformMatrix4fv(uniM, 1, GL_FALSE, value_ptr(meshM));
  // glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(meshV));
  // glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(meshP));

  // texture
  uniTexReflect = myGetUniformLocation(shader, "texReflect");
  uniTexRefract = myGetUniformLocation(shader, "texRefract");
  uniTexDudv = myGetUniformLocation(shader, "texDudv");
  uniTexNormal = myGetUniformLocation(shader, "texNormal");

  glUniform1i(uniTexDudv, 11);
  glUniform1i(uniTexNormal, 12);
  glUniform1i(uniTexReflect, 3);
  glUniform1i(uniTexRefract, 2);

  // light
  uniLightColor = myGetUniformLocation(shader, "lightColor");
  uniLightPos = myGetUniformLocation(shader, "lightPos");

  // glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  // glUniform3fv(uniLightPos, 1, value_ptr(lightPosition));

  // other
  uniDudvMove = myGetUniformLocation(shader, "dudvMove");
  uniCamCoord = myGetUniformLocation(shader, "camCoord");
}
