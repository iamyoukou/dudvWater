#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <GLFW/glfw3.h>
#include <FreeImage.h>

using namespace std;
using namespace glm;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
  // data index
  GLuint v1, v2, v3;
  GLuint vt1, vt2, vt3;
  GLuint vn1, vn2, vn3;
} Face;

class Mesh {
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

  bool isReflect;

  /* Constructors */
  Mesh(const string, bool = false);
  ~Mesh();

  /* Member functions */
  void loadObj(const string);
  void initBuffers();
  void initShader();
  void initUniform();
  void draw(mat4, mat4, mat4, vec3, vec3, vec3, int, int);
  void setTexture(GLuint &, int, const string, FREE_IMAGE_FORMAT);

  void translate(vec3);
  void scale(vec3);
  void rotate(vec3);
  void findAABB();
};

std::string readFile(const std::string);
void printLog(GLuint &);
GLint myGetUniformLocation(GLuint &, string);
GLuint buildShader(string, string, string = "", string = "", string = "");
GLuint compileShader(string, GLenum);
GLuint linkShader(GLuint, GLuint, GLuint, GLuint, GLuint);

#endif
