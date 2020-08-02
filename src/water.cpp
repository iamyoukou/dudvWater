#include "common.h"
#include "water.h"

const float Water::WATER_SIZE = 3.8f;
const float Water::WATER_Y = 2.2f;

Water::Water() {
  initShader();
  initBuffer();
  initTexture();
  initUniform();
  initReflect();
  initRefract();
}

Water::~Water() {}

void Water::draw(mat4 M, mat4 V, mat4 P, vec3 eyePoint, vec3 lightColor,
                 vec3 lightPosition) {
  glUseProgram(shader);

  dudvMove += 0.0005f; // speed
  dudvMove = fmod(dudvMove, 1.0f);

  glUniform1f(uniDudvMove, dudvMove);
  glUniform3fv(uniCamCoord, 1, value_ptr(eyePoint));

  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  glUniform3fv(uniLightPos, 1, value_ptr(lightPosition));

  glUniformMatrix4fv(uniM, 1, GL_FALSE, value_ptr(M));
  glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(V));
  glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(P));

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
  // normals
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0,
                        (GLvoid *)(sizeof(GLfloat) * 6 * (3 + 2)));
  glEnableVertexAttribArray(2);
}

void Water::initShader() {
  shader = buildShader("./shader/vsWater.glsl", "./shader/fsWater.glsl");
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

void Water::initReflect() {
  // framebuffer object
  glGenFramebuffers(1, &fboReflect);
  glBindFramebuffer(GL_FRAMEBUFFER, fboReflect);

  glActiveTexture(GL_TEXTURE0 + 3);
  glGenTextures(1, &tboReflect);
  glBindTexture(GL_TEXTURE_2D, tboReflect);

  // On OSX, must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2, don't know why
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, 0,
               GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, tboReflect, 0);

  // The depth buffer
  // User-defined framebuffer must have a depth buffer to enable depth test
  glGenRenderbuffers(1, &rboDepthReflect);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepthReflect);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH * 2,
                        WINDOW_HEIGHT * 2);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepthReflect);

  glDrawBuffer(GL_COLOR_ATTACHMENT2);
}

void Water::initRefract() {
  // framebuffer object
  glGenFramebuffers(1, &fboRefract);
  glBindFramebuffer(GL_FRAMEBUFFER, fboRefract);

  glActiveTexture(GL_TEXTURE0 + 2);
  glGenTextures(1, &tboRefract);
  glBindTexture(GL_TEXTURE_2D, tboRefract);

  // On OSX, must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2, don't know why
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, 0,
               GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, tboRefract, 0);

  // The depth buffer
  // User-defined framebuffer must have a depth buffer to enable depth test
  glGenRenderbuffers(1, &rboDepthRefract);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepthRefract);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH * 2,
                        WINDOW_HEIGHT * 2);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepthRefract);

  glDrawBuffer(GL_COLOR_ATTACHMENT1);
}

void Water::setTexture(GLuint &tbo, int texUnit, const string texDir,
                       FREE_IMAGE_FORMAT imgType) {
  glActiveTexture(GL_TEXTURE0 + texUnit);

  FIBITMAP *texImage =
      FreeImage_ConvertTo24Bits(FreeImage_Load(imgType, texDir.c_str()));

  glGenTextures(1, &tbo);
  glBindTexture(GL_TEXTURE_2D, tbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(texImage),
               FreeImage_GetHeight(texImage), 0, GL_BGR, GL_UNSIGNED_BYTE,
               (void *)FreeImage_GetBits(texImage));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // release
  FreeImage_Unload(texImage);
}
