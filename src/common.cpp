#include "common.h"

// ================================================
// Read file into a string
// Parameters:
//   fileName: file to read
// Return: string
// ================================================
std::string readFile(const std::string filename)
{
    std::ifstream in;
    in.open(filename.c_str());
    std::stringstream ss;
    ss << in.rdbuf();
    std::string sOut = ss.str();
    in.close();

    return sOut;
}

// =====================================================
// Build shaders
// Parameters:
//   1. vsDir: vertex shader file
//   2. fsDir: fragment shader file
//   3. tcsDir: tessellation control shader file
//   4. tesDir: tessellation evaluation shader file
//   5. geoDir: geometry shader file
// Return: shader executable
// =====================================================
GLuint buildShader(string vsDir, string fsDir, string tcsDir, string tesDir, string geoDir)
{
    // For a shader object, 0 means NULL
    GLuint vs, fs, tcs = 0, tes = 0, geo = 0;
    GLint linkOk;
    GLuint exeShader;

    // Build vertex and fragment shaders
    vs = compileShader(vsDir, GL_VERTEX_SHADER);
    fs = compileShader(fsDir, GL_FRAGMENT_SHADER);

    // (Option) TCS, TES
    if (tcsDir != "" && tesDir != "")
    {
        tcs = compileShader(tcsDir, GL_TESS_CONTROL_SHADER);
        tes = compileShader(tesDir, GL_TESS_EVALUATION_SHADER);
    }

    // (Option) GS
    if (geoDir != "")
    {
        geo = compileShader(geoDir, GL_GEOMETRY_SHADER);
    }

    // Link shader objects
    exeShader = linkShader(vs, fs, tcs, tes, geo);

    return exeShader;
}

// ================================================
// Compile shader file
// Parameters:
//   1. fileName: shader file
//   2. type: shader type
// Return: shader object
// ================================================
GLuint compileShader(string filename, GLenum type)
{
    // Read shader file
    string sTemp = readFile(filename);
    const GLchar *source = sTemp.c_str();

    // Set shader type
    string info;
    switch (type)
    {
        case GL_VERTEX_SHADER:
            info = "Vertex";
            break;
        case GL_FRAGMENT_SHADER:
            info = "Fragment";
            break;
    }

    // If reading shader file fails
    if (source == NULL)
    {
        std::cout << info << " Shader : Can't read shader source file." << std::endl;
        return 0;
    }

    // Compile shader file
    const GLchar *sources[] = {source};
    GLuint objShader = glCreateShader(type);
    glShaderSource(objShader, 1, sources, NULL);
    glCompileShader(objShader);

    // If compiling shader file fails
    GLint compile_ok;
    glGetShaderiv(objShader, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE)
    {
        std::cout << info << " Shader : Fail to compile." << std::endl;
        printLog(objShader);
        glDeleteShader(objShader);
        return 0;
    }

    return objShader;
}

// =======================================================
// Link shaders
// Parameters:
//   1. vsObj: vertex shader object
//   2. fsObj: fragment shader object
//   3. tcsObj: tessellation control shader object
//   4. tesObj: tessellation evaluation shader object
//   5. geoObj: geometry shader object
// Remarks: For a shader object, 0 means NULL
// Return: shader program object
// =======================================================

GLuint linkShader(GLuint vsObj, GLuint fsObj, GLuint tcsObj, GLuint tesObj, GLuint geoObj)
{
    // Attach shader objects to create an executable
    // Then link the executable to rendering pipeline
    GLuint exe = glCreateProgram();
    glAttachShader(exe, vsObj);
    glAttachShader(exe, fsObj);

    // (Option) Attach tessellation shaders
    if (tcsObj != 0 && tesObj != 0)
    {
        glAttachShader(exe, tcsObj);
        glAttachShader(exe, tesObj);
    }

    // (Option) Attach geometry shader
    if (geoObj != 0)
    {
        glAttachShader(exe, geoObj);
    }

    glLinkProgram(exe);

    // Check linking result
    GLint linkOk;
    glGetProgramiv(exe, GL_LINK_STATUS, &linkOk);
    if (linkOk == GL_FALSE)
    {
        std::cout << "Failed to link shader program." << std::endl;
        printLog(exe);
        glDeleteProgram(exe);

        return 0;
    }

    return exe;
}

// ================================================
// Print error log
// Parameters:
//   object: shader object
// ================================================
void printLog(GLuint &object)
{
    // Get compile/build/link log length
    GLint log_length = 0;
    if (glIsShader(object))
    {
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    }
    else if (glIsProgram(object))
    {
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    }
    else
    {
        cerr << "printlog: Not a shader or a program" << endl;
        return;
    }

    // Get compile/build/link log
    // Then output log
    char *log = (char *)malloc(log_length);
    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);
    cerr << log << endl;

    // Release resource
    free(log);
}

// ============================================================================
// Get uniform variable location
//   1. prog: shader program object
//   2. name: uniform variable name (string)
//   3. isWarningOn: output warning message when uniform is not found or not
// ============================================================================
GLint myGetUniformLocation(GLuint &prog, string name, bool isWarningOn)
{
    // If there is no such uniform, glGetUniformLocation returns -1
    GLint location = glGetUniformLocation(prog, name.c_str());
    if (location == -1 && isWarningOn)
    {
        cerr << "Could not bind uniform : " << name << ". "
             << "Did you set the right name? "
             << "Or is " << name << " not used?" << endl;
    }

    return location;
}

// ================================================
// Mesh class
// ================================================
// -----------------------------------------------------
// Constructor
// Parameters:
//   1. fileName: 3D model file path
//   2. reflect: can this object be reflected on water
// -----------------------------------------------------
Mesh::Mesh(const string fileName, bool reflect)
{
    isReflect = reflect;

    // Import mesh with assimp
    scene = importer.ReadFile(fileName, aiProcess_CalcTangentSpace);

    initBuffers();
    initShader();
    initUniform();
}

// -----------------------------------------------------
// Destructor
// -----------------------------------------------------
Mesh::~Mesh()
{
    // Release resource
    // - A mesh consists of several 3D models,
    //   so we must release resource for each model
    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        glDeleteBuffers(1, &vboVtxs[i]);
        glDeleteBuffers(1, &vboUvs[i]);
        glDeleteBuffers(1, &vboNmls[i]);
        glDeleteVertexArrays(1, &vaos[i]);
    }
}

// -----------------------------------------------------
// Initialize shaders
// - Select shader based on isReflect flag
// -----------------------------------------------------
void Mesh::initShader()
{
    string dir = "./shader/";
    string vs, fs;

    if (isReflect)
    {
        vs = dir + "vsReflect.glsl";
        fs = dir + "fsReflect.glsl";
    }
    else
    {
        vs = dir + "vsPhong.glsl";
        fs = dir + "fsPhong.glsl";
    }

    shader = buildShader(vs, fs);
}

// -----------------------------------------------------
// Initialize uniform variables
// -----------------------------------------------------
void Mesh::initUniform()
{
    uniModel = myGetUniformLocation(shader, "M");
    uniView = myGetUniformLocation(shader, "V");
    uniProjection = myGetUniformLocation(shader, "P");
    uniEyePoint = myGetUniformLocation(shader, "eyePoint");
    uniLightColor = myGetUniformLocation(shader, "lightColor");
    uniLightPosition = myGetUniformLocation(shader, "lightPosition");
    uniTexBase = myGetUniformLocation(shader, "texBase");
    uniTexNormal = myGetUniformLocation(shader, "texNormal");

    // If isReflect flag is set,
    // also initialize clipping plane uniform variables
    if (isReflect)
    {
        uniClipPlane0 = myGetUniformLocation(shader, "clipPlane0");
        uniClipPlane1 = myGetUniformLocation(shader, "clipPlane1");
    }
}

// -----------------------------------------------------
// Initialize buffer obect for mesh
// -----------------------------------------------------
void Mesh::initBuffers()
{
    // For each 3D model in the mesh,
    // initialize its vertex attributes.
    // Then create OpenGL contents for them.
    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        const aiMesh *mesh = scene->mMeshes[i];
        int numVtxs = mesh->mNumVertices;

        // numVertices * numComponents
        GLfloat *aVtxCoords = new GLfloat[numVtxs * 3];
        GLfloat *aUvs = new GLfloat[numVtxs * 2];
        GLfloat *aNormals = new GLfloat[numVtxs * 3];

        for (size_t j = 0; j < numVtxs; j++)
        {
            aiVector3D &vtx = mesh->mVertices[j];
            aVtxCoords[j * 3 + 0] = vtx.x;
            aVtxCoords[j * 3 + 1] = vtx.y;
            aVtxCoords[j * 3 + 2] = vtx.z;

            aiVector3D &nml = mesh->mNormals[j];
            aNormals[j * 3 + 0] = nml.x;
            aNormals[j * 3 + 1] = nml.y;
            aNormals[j * 3 + 2] = nml.z;

            aiVector3D &uv = mesh->mTextureCoords[0][j];
            aUvs[j * 2 + 0] = uv.x;
            aUvs[j * 2 + 1] = uv.y;
        }

        // vao
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        vaos.push_back(vao);

        // vbo for vertex
        GLuint vboVtx;
        glGenBuffers(1, &vboVtx);
        glBindBuffer(GL_ARRAY_BUFFER, vboVtx);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVtxs * 3, aVtxCoords, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        vboVtxs.push_back(vboVtx);

        // vbo for uv
        GLuint vboUv;
        glGenBuffers(1, &vboUv);
        glBindBuffer(GL_ARRAY_BUFFER, vboUv);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVtxs * 2, aUvs, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        vboUvs.push_back(vboUv);

        // vbo for normal
        GLuint vboNml;
        glGenBuffers(1, &vboNml);
        glBindBuffer(GL_ARRAY_BUFFER, vboNml);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVtxs * 3, aNormals, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);
        vboNmls.push_back(vboNml);

        // Release resources
        delete[] aVtxCoords;
        delete[] aUvs;
        delete[] aNormals;
    }
}

// -----------------------------------------------------
// Set texture object
// Parameters:
//   1. tbo: texture buffer object
//   2. texUnit: texture unit to use
//   3. texDir: texture image file path
//   4. imgType: texture image type
// -----------------------------------------------------
void Mesh::setTexture(GLuint &tbo, int texUnit, const string texDir, FREE_IMAGE_FORMAT imgType)
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

// --------------------------------------------------------------
// Draw mesh
// Parameters:
//   1. M, V, P: model, view, projection transformation matrix
//   2. eye: eye position
//   3. lightColor, lightPosition: lighting configuration
//   4. uniBaseColor: base color texture
//   5. uniNormal: normal map
// --------------------------------------------------------------
void Mesh::draw(mat4 M, mat4 V, mat4 P, vec3 eye, vec3 lightColor, vec3 lightPosition, int uniBaseColor, int uniNormal)
{
    // Bind shader program
    glUseProgram(shader);

    // Set transformation matrices
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(M));
    glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(V));
    glUniformMatrix4fv(uniProjection, 1, GL_FALSE, value_ptr(P));

    // Set eye point
    glUniform3fv(uniEyePoint, 1, value_ptr(eye));

    // Set lighting
    glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
    glUniform3fv(uniLightPosition, 1, value_ptr(lightPosition));

    // Set textures
    glUniform1i(uniTexBase, uniBaseColor);
    glUniform1i(uniTexNormal, uniNormal);

    // Draw mesh (draw each 3D model in the mesh)
    for (size_t i = 0; i < scene->mNumMeshes; i++)
    {
        int numVtxs = scene->mMeshes[i]->mNumVertices;

        glBindVertexArray(vaos[i]);
        glDrawArrays(GL_TRIANGLES, 0, numVtxs);
    }
}
