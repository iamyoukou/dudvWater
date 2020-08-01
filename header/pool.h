#ifndef POOL_H
#define POOL_H

#include "common.h"

class Pool {
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
  GLuint tboBase, tboNormal;
  GLint uniModel, uniView, uniProjection;
  GLint uniEyePoint, uniLightColor, uniLightPosition;
  GLint uniTexBase, uniTexNormal;
  GLint uniClipPlane0, uniClipPlane1;

  // aabb
  vec3 min, max;

  mat4 model, view, projection;

  /* Constructors */
  Pool(const string);
  ~Pool();

  /* Member functions */
  void loadObj(const string);
  void initBuffers();
  void initShader();
  void initUniform();
  void draw(mat4, mat4, mat4, vec3, vec3, vec3, int, int);
  void setTexture(GLuint &, int, const string, FREE_IMAGE_FORMAT);
};

#endif
