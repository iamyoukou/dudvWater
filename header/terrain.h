#ifndef TERRAIN_H
#define TERRAIN_H

#include "common.h"

typedef struct {
  // data index
  GLuint v1, v2, v3, v4;
  GLuint vt1, vt2, vt3, vt4;
  GLuint vn1, vn2, vn3, vn4;
} Face;

class Terrain {
public:
  // mesh data
  vector<vec3> vertices;
  vector<vec2> uvs;
  vector<vec3> faceNormals;
  vector<Face> faces;

  // opengl data
  GLuint vboVtxs, vboUvs, vboNormals;
  GLuint vao;
  GLuint shader;
  GLuint tboBase, tboNormal, tboHeight;
  GLint uniModel, uniView, uniProjection;
  GLint uniEyePoint, uniLightColor, uniLightPosition;
  GLint uniTexBase, uniTexNormal, uniTexHeight;
  GLint uniClipPlane0, uniClipPlane1;

  mat4 model, view, projection;

  /* Constructors */
  Terrain(const string);
  ~Terrain();

  /* Member functions */
  void loadObj(const string);
  void initBuffers();
  void initShader();
  void initUniform();
  void draw(mat4, mat4, mat4, vec3, vec3, vec3, int, int, int);
  void setTexture(GLuint &, int, const string, FREE_IMAGE_FORMAT);
};

#endif
