#ifndef SKYBOX_H
#define SKYBOX_H

#include "common.h"

class Skybox
{
  public:
    // ----------------------------------------------------
    // Skybox vertices
    // - 6 faces, 2 triangles per face,
    //   3 vertices per triangle, 3 coordinates per vertex
    // ----------------------------------------------------
    float SKYBOX_SIZE = 500.f;
    GLfloat vtxs[108] = {-SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
                         -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, //
                         SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  SKYBOX_SIZE,
                         -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE,

                         -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE,
                         -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, //
                         -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
                         SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,

                         SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE,
                         SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE, //
                         SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
                         -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE,

                         -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE,
                         SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE, //
                         SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE,
                         SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,

                         -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE,  SKYBOX_SIZE,
                         -SKYBOX_SIZE, SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE, //
                         SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE,
                         SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE,

                         -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
                         SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, //
                         SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
                         SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE};

    // -----------------------------------------
    // OpenGL objects
    // -----------------------------------------
    GLuint vbo, tbo, vao, shader;
    GLint uniM, uniV, uniP;

    // -----------------------------------------
    // Transformation matrices
    // -----------------------------------------
    mat4 M, V, P;

    // -----------------------------------------
    // Constructor and destructor
    // -----------------------------------------
    Skybox();
    ~Skybox();

    // -----------------------------------------
    // Member functions
    // -----------------------------------------
    void draw(mat4, mat4, mat4, vec3);
    void initTexture();
    void initBuffer();
    void initShader();
    void initUniform();
};

#endif
