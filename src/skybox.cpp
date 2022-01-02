#include "skybox.h"

// -----------------------------------------
// Constructor
// -----------------------------------------
Skybox::Skybox()
{
    initShader();
    initUniform();
    initTexture();
    initBuffer();
}

// -----------------------------------------
// Destructor
// -----------------------------------------
Skybox::~Skybox() {}

// ----------------------------------------------------
// Draw skybox
// - model, view, projection: transformation matrices
// ----------------------------------------------------
void Skybox::draw(mat4 model, mat4 view, mat4 projection, vec3 eyePoint)
{
    glUseProgram(shader);

    glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(view));
    glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(projection));

    // Let the center of the skybox always at eyePoint
    // NOTE: the matrix of GLM is column major
    M = model;
    M[3][0] += eyePoint.x;
    M[3][1] += eyePoint.y;
    M[3][2] += eyePoint.z;
    glUniformMatrix4fv(uniM, 1, GL_FALSE, value_ptr(M));

    glUseProgram(shader);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// ----------------------------------------------------
// Initialize cubemap
// ----------------------------------------------------
void Skybox::initTexture()
{
    // Create texture object
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tbo);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tbo);

    // Parameter settings
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Read images into cubemap
    vector<string> texImages;
    texImages.push_back("./image/right.png");
    texImages.push_back("./image/left.png");
    texImages.push_back("./image/bottom.png");
    texImages.push_back("./image/top.png");
    texImages.push_back("./image/back.png");
    texImages.push_back("./image/front.png");

    for (GLuint i = 0; i < texImages.size(); i++)
    {
        FIBITMAP *image = FreeImage_ConvertTo24Bits(FreeImage_Load(FIF_PNG, texImages[i].c_str()));
        int width = FreeImage_GetWidth(image);
        int height = FreeImage_GetHeight(image);

        // Set image to each face of a cubemap
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE,
                     (void *)FreeImage_GetBits(image));

        FreeImage_Unload(image);
    }
}

// ----------------------------------------------------
// Initialize buffer object
// ----------------------------------------------------
void Skybox::initBuffer()
{
    // If put these code before setting texture,
    // no skybox will be rendered
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 6 * 3, vtxs, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

// ----------------------------------------------------
// Initialize shaders
// ----------------------------------------------------
void Skybox::initShader()
{
    // Build vertex and fragment shaders
    shader = buildShader("./shader/vsSkybox.glsl", "./shader/fsSkybox.glsl");
}

// ----------------------------------------------------
// Initialize uniform variables
// ----------------------------------------------------
void Skybox::initUniform()
{
    glUseProgram(shader);

    uniM = myGetUniformLocation(shader, "M");
    uniV = myGetUniformLocation(shader, "V");
    uniP = myGetUniformLocation(shader, "P");

    glUniformMatrix4fv(uniM, 1, GL_FALSE, value_ptr(M));
    glUniformMatrix4fv(uniV, 1, GL_FALSE, value_ptr(V));
    glUniformMatrix4fv(uniP, 1, GL_FALSE, value_ptr(P));
}
