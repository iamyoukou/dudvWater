#ifndef WATER_H
#define WATER_H

#include "common.h"

class Water
{
  public:
    // Water surface mesh constants
    static const float WATER_SIZE;
    static const float WATER_Y;

    // Water moving speed
    static float dudvMove;

    // -----------------------------------------------------
    // Water surface mesh
    // - One quad, two triangles
    // - 3 vertices per triangle, 3 coordinates per vertex
    // -----------------------------------------------------
    GLfloat vtxs[48] = {
        // Triangle 1 vertices
        -WATER_SIZE, WATER_Y, -WATER_SIZE, -WATER_SIZE, WATER_Y, WATER_SIZE, WATER_SIZE, WATER_Y, WATER_SIZE,
        // Triangle 2 vertices
        WATER_SIZE, WATER_Y, WATER_SIZE, WATER_SIZE, WATER_Y, -WATER_SIZE, -WATER_SIZE, WATER_Y, -WATER_SIZE,
        // UV coordinates
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Vertex normals
        0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f};

    // -----------------------------------------------------
    // OpenGL objects
    // -----------------------------------------------------
    // Vertex buffer object
    GLuint vbo;

    // Vertex attribute object
    GLuint vao;

    // Texture buffer object for dudv map and normal map
    GLuint tboDudv, tboNormal;

    // Uniforms for transformation matrices
    GLint uniM, uniV, uniP;

    // Uniforms for lighting
    GLint uniLightColor, uniLightPos;

    // Uniforms for textures
    // - For reflection, refraction, dudv, normal, refraction depth map and cubemap
    GLint uniTexReflect, uniTexRefract, uniTexDudv, uniTexNormal, uniTexDepthRefr, uniTexSkybox;

    // Uniforms for dudv moving speed and eye point
    GLint uniDudvMove, uniEyePoint;

    // Shader object
    GLuint shader;

    // Texture buffer objects
    // - For refraction, reflection and refraction depth map
    GLuint tboRefract, tboReflect, tboDepthRefr;

    // Frame buffer objects
    // - For refraction and reflection map
    GLuint fboRefract, fboReflect;

    // Render buffer objects
    // - For refraction depth and reflection depth map
    GLuint rboDepthRefract, rboDepthReflect;

    // -----------------------------------------------------
    // Constructor and destructor
    // -----------------------------------------------------
    Water();
    ~Water();

    // -----------------------------------------------------
    // Member functions
    // -----------------------------------------------------
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
