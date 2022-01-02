#ifndef COMMON_H
#define COMMON_H

// =======================================
// Headers: order matters
// =======================================
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
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <GLFW/glfw3.h>
#include <FreeImage.h>

using namespace std;
using namespace glm;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// =======================================
// Mesh class definition
// =======================================
class Mesh
{
  public:
    // Assimp objects to import 3D object
    Assimp::Importer importer;
    const aiScene *scene;

    // ------------------------------------------------
    // OpenGL object list
    // - vtx: vertex position
    // - uv: uv coordinate
    // - nml: surface normal
    // ------------------------------------------------
    vector<GLuint> vboVtxs, vboUvs, vboNmls, vaos;

    // ------------------------------------------------
    // OpenGL object for shaders
    // ------------------------------------------------
    GLuint shader;
    GLuint tboBase, tboNormal;
    GLint uniModel, uniView, uniProjection;
    GLint uniEyePoint, uniLightColor, uniLightPosition;
    GLint uniTexBase, uniTexNormal;
    GLint uniClipPlane0, uniClipPlane1;

    // Transformation matrices
    mat4 model, view, projection;

    // Can this object be reflected on water surface
    bool isReflect;

    // ------------------------------------------------
    // Constructor and destructor
    // ------------------------------------------------
    Mesh(const string, bool = false);
    ~Mesh();

    // ------------------------------------------------
    // Member functions
    // ------------------------------------------------
    void initBuffers();
    void initShader();
    void initUniform();
    void draw(mat4, mat4, mat4, vec3, vec3, vec3, int, int);
    void setTexture(GLuint &, int, const string, FREE_IMAGE_FORMAT);
};

// =======================================
// OpenGL utilities
// =======================================
std::string readFile(const std::string);
void printLog(GLuint &);
GLint myGetUniformLocation(GLuint &, string, bool = false);
GLuint buildShader(string, string, string = "", string = "", string = "");
GLuint compileShader(string, GLenum);
GLuint linkShader(GLuint, GLuint, GLuint, GLuint, GLuint);

#endif
