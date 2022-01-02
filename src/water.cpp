#include "common.h"
#include "water.h"

const float Water::WATER_SIZE = 1.f;
const float Water::WATER_Y = 2.2f;
float Water::dudvMove = 0.f;

// -----------------------------------------------------
// Constructor
// -----------------------------------------------------
Water::Water()
{
    initShader();
    initBuffer();
    initTexture();
    initUniform();
    initReflect();
    initRefract();
}

// -----------------------------------------------------
// Destructor
// -----------------------------------------------------
Water::~Water() {}

// ---------------------------------------------------------------
// Draw water surface
//   1. M, V, P: model, view, projection transformation matrix
//   2. eye: eye position
//   3. lightColor, lightPosition: lighting configuration
// ---------------------------------------------------------------
void Water::draw(mat4 M, mat4 V, mat4 P, vec3 eyePoint, vec3 lightColor, vec3 lightPosition)
{
    // Bind shader program
    glUseProgram(shader);

    // Set dudv moving speed
    glUniform1f(uniDudvMove, dudvMove);

    // Set eye point
    glUniform3fv(uniEyePoint, 1, value_ptr(eyePoint));

    // Set lighting
    glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
    glUniform3fv(uniLightPos, 1, value_ptr(lightPosition));

    // Set transformation matrices
    glUniformMatrix4fv(uniM, 1, GL_FALSE, value_ptr(M));
    glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(V));
    glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(P));

    // Draw mesh
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// -----------------------------------------------------
// Initialize buffer obect
// -----------------------------------------------------
void Water::initBuffer()
{
    // Create vertex attribute object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtxs), vtxs, GL_STATIC_DRAW);

    // Set position info
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Set uv info
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(GLfloat) * 6 * 3));
    glEnableVertexAttribArray(1);

    // Set normal info
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(GLfloat) * 6 * (3 + 2)));
    glEnableVertexAttribArray(2);
}

// -----------------------------------------------------
// Initialize shaders
// -----------------------------------------------------
void Water::initShader() { shader = buildShader("./shader/vsWater.glsl", "./shader/fsWater.glsl"); }

// -----------------------------------------------------
// Initialize textures
// -----------------------------------------------------
void Water::initTexture()
{
    // Dudv map
    setTexture(tboDudv, 10, "./image/fftDudv.png", FIF_PNG);

    // Normal map
    setTexture(tboNormal, 11, "./image/fftNormal.png", FIF_PNG);
}

// -----------------------------------------------------
// Initialize uniform variables
// - For uniforms that have constant values,
//   also initialize their values
// -----------------------------------------------------
void Water::initUniform()
{
    // Bind shader program
    glUseProgram(shader);

    // Transformation matrices
    uniM = myGetUniformLocation(shader, "M");
    uniV = myGetUniformLocation(shader, "V");
    uniP = myGetUniformLocation(shader, "P");

    // Texture
    uniTexReflect = myGetUniformLocation(shader, "texReflect");
    uniTexRefract = myGetUniformLocation(shader, "texRefract");
    uniTexSkybox = myGetUniformLocation(shader, "texSkybox");
    uniTexDudv = myGetUniformLocation(shader, "texDudv");
    uniTexNormal = myGetUniformLocation(shader, "texNormal");
    uniTexDepthRefr = myGetUniformLocation(shader, "texDepthRefr");

    glUniform1i(uniTexDudv, 10);
    glUniform1i(uniTexNormal, 11);
    glUniform1i(uniTexReflect, 3);
    glUniform1i(uniTexRefract, 2);
    glUniform1i(uniTexDepthRefr, 25);

    // Lighting
    uniLightColor = myGetUniformLocation(shader, "lightColor");
    uniLightPos = myGetUniformLocation(shader, "lightPos");

    // Dudv moving speed
    uniDudvMove = myGetUniformLocation(shader, "dudvMove");

    // Eye point
    uniEyePoint = myGetUniformLocation(shader, "eyePoint");
}

// -----------------------------------------------------
// Initialize reflection texture
// -----------------------------------------------------
void Water::initReflect()
{
    // Create framebuffer object
    glGenFramebuffers(1, &fboReflect);
    glBindFramebuffer(GL_FRAMEBUFFER, fboReflect);

    // Create texture object
    glActiveTexture(GL_TEXTURE0 + 3);
    glGenTextures(1, &tboReflect);
    glBindTexture(GL_TEXTURE_2D, tboReflect);

    // On macOS, must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2, don't know why
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set texture object
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, tboReflect, 0);

    // The depth buffer
    // User-defined framebuffer must have a depth buffer to enable depth test
    glGenRenderbuffers(1, &rboDepthReflect);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthReflect);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthReflect);

    // Attach framebuffer
    glDrawBuffer(GL_COLOR_ATTACHMENT2);
}

// -----------------------------------------------------
// Initialize refraction texture
// -----------------------------------------------------
void Water::initRefract()
{
    // Create framebuffer object
    glGenFramebuffers(1, &fboRefract);
    glBindFramebuffer(GL_FRAMEBUFFER, fboRefract);

    // Create texture object
    glActiveTexture(GL_TEXTURE0 + 2);
    glGenTextures(1, &tboRefract);
    glBindTexture(GL_TEXTURE_2D, tboRefract);

    // On macOS, must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2, don't know why
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, tboRefract, 0);

    // The depth buffer
    // User-defined framebuffer must have a depth buffer to enable depth test
    // The following setting write depth into framebuffer
    // glGenRenderbuffers(1, &rboDepthRefract);
    // glBindRenderbuffer(GL_RENDERBUFFER, rboDepthRefract);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH *
    // 2,
    //                       WINDOW_HEIGHT * 2);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    //                           GL_RENDERBUFFER, rboDepthRefract);

    // The depth buffer
    // User-defined framebuffer must have a depth buffer to enable depth test
    // The following setting write depth into texture, and can be exported to an image
    glActiveTexture(GL_TEXTURE0 + 25);
    glGenTextures(1, &tboDepthRefr);
    glBindTexture(GL_TEXTURE_2D, tboDepthRefr);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tboDepthRefr, 0);

    // Attach framebuffer
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
}

// -----------------------------------------------------
// Set texture object
// Parameters:
//   1. tbo: texture buffer object
//   2. texUnit: texture unit to use
//   3. texDir: texture image file path
//   4. imgType: texture image type
// -----------------------------------------------------
void Water::setTexture(GLuint &tbo, int texUnit, const string texDir, FREE_IMAGE_FORMAT imgType)
{
    // Always use "GL_TEXTURE0 + N" to specify a texture unit
    glActiveTexture(GL_TEXTURE0 + texUnit);

    // Create texture image from file
    FIBITMAP *texImage = FreeImage_ConvertTo24Bits(FreeImage_Load(imgType, texDir.c_str()));

    // Create texture object for the image
    glGenTextures(1, &tbo);
    glBindTexture(GL_TEXTURE_2D, tbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(texImage), FreeImage_GetHeight(texImage), 0, GL_BGR,
                 GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(texImage));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Release resource
    FreeImage_Unload(texImage);
}
