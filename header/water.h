#ifndef WATER_H
#define WATER_H

#include "common.h"

class Water {
public:
  const float WATER_SIZE = 3.8f;
  const float WATER_Y = 2.2f;
  float dudvMove = 0.f;

  GLfloat vtxs[48] = {
      // coords
      -WATER_SIZE, WATER_Y, -WATER_SIZE, -WATER_SIZE, WATER_Y, WATER_SIZE,
      WATER_SIZE, WATER_Y, WATER_SIZE, WATER_SIZE, WATER_Y, WATER_SIZE,
      WATER_SIZE, WATER_Y, -WATER_SIZE, -WATER_SIZE, WATER_Y, -WATER_SIZE,
      // texture coords for dudv
      1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      // normals
      0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f,
      0.f, 1.f, 0.f};

  GLuint vbo;
  GLuint tboDudv, tboNormal;
  GLuint vao;
  GLint uniM, uniV, uniP;
  GLint uniLightColor, uniLightPos;
  GLint uniTexReflect, uniTexRefract, uniTexDudv, uniTexNormal;
  GLint uniDudvMove, uniCamCoord;
  GLuint shader;
  GLuint tboRefract, tboReflect;
  GLuint fboRefract, fboReflect;

  Water();
  ~Water();

  void draw(mat4, mat4, mat4, vec3, vec3, vec3);
  void initBuffer();
  void initShader();
  void initTexture();
  void initUniform();
  void initReflect();
  void initRefract();
  void setTexture(GLuint &, int, const string, FREE_IMAGE_FORMAT);
};

#endif
