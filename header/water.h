#ifndef WATER_H
#define WATER_H

class Water {
public:
  const float WATER_SIZE = 3.8f;
  const float WATER_Y = 2.2f;
  GLfloat vtxs[30] = {
      // coords
      -WATER_SIZE, WATER_Y, -WATER_SIZE, -WATER_SIZE, WATER_Y, WATER_SIZE,
      WATER_SIZE, WATER_Y, WATER_SIZE, WATER_SIZE, WATER_Y, WATER_SIZE,
      WATER_SIZE, WATER_Y, -WATER_SIZE, -WATER_SIZE, WATER_Y, -WATER_SIZE,
      // texture coords for dudv
      1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

  GLuint vbo;
  GLuint tboDudv, tboNormal;
  GLuint vao;
  GLint uniM, uniV, uniP;
  GLint uniLightColor, uniLightPos;
  GLint uniTexReflect, uniTexRefract, uniTexDudv, uniTexNormal;
  GLint uniDudvMove, uniCamCoord;
  GLuint shader;

  Water();
  ~Water();

  void draw(mat4, mat4, mat4, vec3, vec3, vec3, float);
  void initBuffer();
  void initShader();
  void initTexture();
  void initUniform();
};

#endif
