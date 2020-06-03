#include "common.h"

GLFWwindow *window;

bool saveTrigger = false;
int faceNumber;
int frameNumber = 0;

float verticalAngle = -2.46533f;
float horizontalAngle = 6.27918f;
float initialFoV = 45.0f;
float speed = 5.0f;
float mouseSpeed = 0.005f;
float farPlane = 2000.f;
float dudvMove = 0.f;

vec3 eyePoint = vec3(8.652440, 12.537420, -4.424253);
vec3 eyeDirection =
    vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
         sin(verticalAngle) * sin(horizontalAngle));
vec3 up = vec3(0.f, 1.f, 0.f);

float verticalAngleReflect = 3.14 - verticalAngle;
float horizontalAngleReflect = horizontalAngle;
vec3 eyePointReflect = vec3(eyePoint.x, -eyePoint.y, eyePoint.z);
vec3 eyeDirectionReflect;

vec3 lightPosition = vec3(3.f, 3.f, 3.f);
vec3 lightColor = vec3(1.f, 1.f, 1.f);
float lightPower = 12.f;

vec3 materialDiffuse = vec3(0.1f, 0.1f, 0.1f);
vec3 materialAmbient = vec3(0.1f, 0.1f, 0.1f);
vec3 materialSpecular = vec3(1.f, 1.f, 1.f);

const float SKYBOX_SIZE = 500.f;
GLfloat vtxsSkybox[] = {
    -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE,

    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE,
    SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,

    SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE,
    SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
    SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE,
    SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE,

    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
    SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,

    -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE,  SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
    SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE,  SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE,  -SKYBOX_SIZE,

    -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    SKYBOX_SIZE,  SKYBOX_SIZE,  -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE,  -SKYBOX_SIZE, SKYBOX_SIZE};

const float WATER_SIZE = 3.8f;
const float WATER_Y = 2.2f;
GLfloat vtxsWater[] = {
    // coords
    -WATER_SIZE, WATER_Y, -WATER_SIZE, -WATER_SIZE, WATER_Y, WATER_SIZE,
    WATER_SIZE, WATER_Y, WATER_SIZE, WATER_SIZE, WATER_Y, WATER_SIZE,
    WATER_SIZE, WATER_Y, -WATER_SIZE, -WATER_SIZE, WATER_Y, -WATER_SIZE,
    // texture coords for dudv
    1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

GLuint vboSkybox, tboSkybox;
GLuint tboRefract, tboReflect;
GLuint vboWater;
GLuint tboWaterDudv, tboWaterNormal;
GLuint vaoSkybox, vaoWater;
GLuint fboRefract, fboReflect;
GLint uniSkyboxM, uniSkyboxV, uniSkyboxP;
GLint uniPoolM, uniPoolV, uniPoolP;
GLint uniWaterM, uniWaterV, uniWaterP;
GLint uniLightColor, uniLightPos, uniLightPower, uniLightDir;
GLint uniDiffuse, uniAmbient, uniSpecular;
GLint uniPoolTexBase;
GLint uniTexRefract, uniTexReflect, uniTexDudv, uniTexNormal;
GLint uniDudvMove;
GLint uniCamCoord;
GLint uniWaterLightColor, uniWaterLightPos;
mat4 meshM, meshV, meshP;
mat4 skyboxM, skyboxV, skyboxP, oriSkyboxM;
mat4 reflectV;
GLuint shaderSkybox, shaderPool, shaderWater;
GLuint tboPoolBase;

Mesh pool;

void computeMatricesFromInputs();
void keyCallback(GLFWwindow *, int, int, int, int);
void initGL();
void initOther();
void initShader();
void initTexture();
void initMatrix();
void initUniform();
void initSkybox();
void initMesh();
void initRefract();
void initReflect();
void drawMesh();
void drawWater();
void drawSkybox();
GLuint createTexture(GLuint, string, FREE_IMAGE_FORMAT);

int main(int argc, char **argv) {
  initGL();
  initOther();
  initShader();
  initTexture();
  initMatrix();
  initUniform();

  initSkybox();
  initMesh();
  initRefract();
  initReflect();

  // a rough way to solve cursor position initialization problem
  // must call glfwPollEvents once to activate glfwSetCursorPos
  // this is a glfw mechanism problem
  glfwPollEvents();
  glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClearColor(97 / 256.f, 175 / 256.f, 239 / 256.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // view control
    computeMatricesFromInputs();

    /* render to refraction texture */
    glBindFramebuffer(GL_FRAMEBUFFER, fboRefract);

    // clipping
    glEnable(GL_CLIP_DISTANCE0);
    glDisable(GL_CLIP_DISTANCE1);

    vec4 clipPlane0 = vec4(0, 1, 0, -2.2);
    glUseProgram(shaderPool);
    GLuint uniClipPlane0 = myGetUniformLocation(shaderPool, "clipPlane0");
    glUniform4fv(uniClipPlane0, 1, value_ptr(clipPlane0));

    // draw scene
    drawSkybox();
    drawMesh();

    /* render to reflection texture */
    glBindFramebuffer(GL_FRAMEBUFFER, fboReflect);

    // clipping
    glDisable(GL_CLIP_DISTANCE0);
    glEnable(GL_CLIP_DISTANCE1);

    glUseProgram(shaderSkybox);
    glUniformMatrix4fv(uniSkyboxV, 1, GL_FALSE, value_ptr(reflectV));

    glUseProgram(shaderPool);
    glUniformMatrix4fv(shaderPool, 1, GL_FALSE, value_ptr(reflectV));

    vec4 clipPlane1 = vec4(0, 1, 0, -3);
    GLuint uniClipPlane1 = myGetUniformLocation(shaderPool, "clipPlane1");
    glUniform4fv(uniClipPlane1, 1, value_ptr(clipPlane1));

    // draw scene
    drawSkybox();
    drawMesh();

    /* render to main screen */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_CLIP_DISTANCE0);
    glDisable(GL_CLIP_DISTANCE1);

    glUseProgram(shaderSkybox);
    glUniformMatrix4fv(uniSkyboxV, 1, GL_FALSE, value_ptr(skyboxV));

    glUseProgram(shaderPool);
    glUniformMatrix4fv(uniPoolV, 1, GL_FALSE, value_ptr(meshV));

    // draw scene
    drawSkybox();
    drawMesh();
    drawWater();

    // refresh frame
    glfwSwapBuffers(window);

    // if (saveTrigger) {
    //   string dir = "./result/output";
    //   // zero padding
    //   // e.g. "output0001.bmp"
    //   string num = to_string(frameNumber);
    //   num = string(4 - num.length(), '0') + num;
    //   string output = dir + num + ".bmp";
    //
    //   FIBITMAP *outputImage =
    //       FreeImage_AllocateT(FIT_UINT32, WINDOW_WIDTH * 2, WINDOW_HEIGHT *
    //       2);
    //   glReadPixels(0, 0, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, GL_BGRA,
    //                GL_UNSIGNED_INT_8_8_8_8_REV,
    //                (GLvoid *)FreeImage_GetBits(outputImage));
    //   FreeImage_Save(FIF_BMP, outputImage, output.c_str(), 0);
    //   std::cout << output << " saved." << '\n';
    //   frameNumber++;
    // }

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();

  // FreeImage library
  FreeImage_DeInitialise();

  return EXIT_SUCCESS;
}

void computeMatricesFromInputs() {
  // glfwGetTime is called only once, the first time this function is called
  static float lastTime = glfwGetTime();

  // Compute time difference between current and last frame
  float currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  // Reset mouse position for next frame
  glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  // Compute new orientation
  // As the cursor is put at the center of the screen,
  // (WINDOW_WIDTH/2.f - xpos) and (WINDOW_HEIGHT/2.f - ypos) are offsets
  horizontalAngle += mouseSpeed * float(xpos - WINDOW_WIDTH / 2.f);
  verticalAngle += mouseSpeed * float(-ypos + WINDOW_HEIGHT / 2.f);

  horizontalAngleReflect += mouseSpeed * float(xpos - WINDOW_WIDTH / 2.f);
  verticalAngleReflect += mouseSpeed * float(-ypos + WINDOW_HEIGHT / 2.f);

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  vec3 direction =
      vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
           sin(verticalAngle) * sin(horizontalAngle));

  vec3 directionReflect =
      vec3(sin(verticalAngleReflect) * cos(horizontalAngleReflect),
           cos(verticalAngleReflect),
           sin(verticalAngleReflect) * sin(horizontalAngleReflect));

  // Right vector
  vec3 right = vec3(cos(horizontalAngle - 3.14 / 2.f), 0.f,
                    sin(horizontalAngle - 3.14 / 2.f));

  vec3 rightReflect = vec3(cos(horizontalAngleReflect - 3.14 / 2.f), 0.f,
                           sin(horizontalAngleReflect - 3.14 / 2.f));

  // new up vector
  vec3 newUp = cross(right, direction);

  vec3 newUpReflect = cross(rightReflect, directionReflect);

  // Move forward
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    eyePoint += direction * deltaTime * speed;

    eyePointReflect += directionReflect * deltaTime * speed;
  }
  // Move backward
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    eyePoint -= direction * deltaTime * speed;

    eyePointReflect -= directionReflect * deltaTime * speed;
  }
  // Strafe right
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    eyePoint += right * deltaTime * speed;

    eyePointReflect += rightReflect * deltaTime * speed;
  }
  // Strafe left
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    eyePoint -= right * deltaTime * speed;

    eyePointReflect -= rightReflect * deltaTime * speed;
  }

  mat4 newV = lookAt(eyePoint, eyePoint + direction, newUp);
  mat4 newP = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f,
                          farPlane);

  // for reflect
  reflectV =
      lookAt(eyePointReflect, eyePointReflect + directionReflect, newUpReflect);

  // update for skybox
  glUseProgram(shaderSkybox);
  skyboxV = newV;
  skyboxP = newP;
  glUniformMatrix4fv(uniSkyboxV, 1, GL_FALSE, value_ptr(skyboxV));
  glUniformMatrix4fv(uniSkyboxP, 1, GL_FALSE, value_ptr(skyboxP));

  // Let the center of the skybox always at eyePoint
  // CAUTION: the matrix of GLM is column major
  skyboxM[3][0] = oriSkyboxM[0][3] + eyePoint.x;
  skyboxM[3][1] = oriSkyboxM[1][3] + eyePoint.y;
  skyboxM[3][2] = oriSkyboxM[2][3] + eyePoint.z;
  glUniformMatrix4fv(uniSkyboxM, 1, GL_FALSE, value_ptr(skyboxM));

  // update for pool
  glUseProgram(shaderPool);
  meshV = newV;
  meshP = newP;
  glUniformMatrix4fv(uniPoolV, 1, GL_FALSE, value_ptr(meshV));
  glUniformMatrix4fv(uniPoolP, 1, GL_FALSE, value_ptr(meshP));

  // update for water
  glUseProgram(shaderWater);
  glUniformMatrix4fv(uniWaterV, 1, GL_FALSE, value_ptr(meshV));
  glUniformMatrix4fv(uniWaterP, 1, GL_FALSE, value_ptr(meshP));

  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;
}

void keyCallback(GLFWwindow *keyWnd, int key, int scancode, int action,
                 int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_ESCAPE: {
      glfwSetWindowShouldClose(keyWnd, GLFW_TRUE);
      break;
    }
    case GLFW_KEY_F: {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
    }
    case GLFW_KEY_L: {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    }
    case GLFW_KEY_I: {
      // std::cout << "eyePoint: " << to_string(eyePoint) << '\n';
      // std::cout << "verticleAngle: " << fmod(verticalAngle, 6.28f) << ", "
      //           << "horizontalAngle: " << fmod(horizontalAngle, 6.28f) <<
      //           endl;
      std::cout << to_string(eyeDirectionReflect) << '\n';

      break;
    }
    case GLFW_KEY_Y: {
      saveTrigger = !saveTrigger;
      frameNumber = 0;
      break;
    }
    default:
      break;
    }
  }
}

void initSkybox() {
  // texture
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &tboSkybox);
  glBindTexture(GL_TEXTURE_CUBE_MAP, tboSkybox);

  // parameter setting
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // read images into cubemap
  vector<string> texImages;
  texImages.push_back("./image/right.png");
  texImages.push_back("./image/left.png");
  texImages.push_back("./image/bottom.png");
  texImages.push_back("./image/top.png");
  texImages.push_back("./image/back.png");
  texImages.push_back("./image/front.png");

  for (GLuint i = 0; i < texImages.size(); i++) {
    int width, height;
    FIBITMAP *image;

    image = FreeImage_ConvertTo24Bits(
        FreeImage_Load(FIF_PNG, texImages[i].c_str()));

    width = FreeImage_GetWidth(image);
    height = FreeImage_GetHeight(image);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                 0, GL_BGR, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(image));

    FreeImage_Unload(image);
  }

  // vbo
  // if put these code before setting texture,
  // no skybox will be rendered
  // don't know why
  glGenVertexArrays(1, &vaoSkybox);
  glBindVertexArray(vaoSkybox);

  glGenBuffers(1, &vboSkybox);
  glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 6 * 3, vtxsSkybox,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
}

void initMesh() {
  // pool
  pool = loadObj("./mesh/pool.obj");
  createMesh(pool);

  // water
  glGenVertexArrays(1, &vaoWater);
  glBindVertexArray(vaoWater);

  glGenBuffers(1, &vboWater);
  glBindBuffer(GL_ARRAY_BUFFER, vboWater);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vtxsWater), vtxsWater, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
                        (GLvoid *)(sizeof(GLfloat) * 6 * 3));
  glEnableVertexAttribArray(1);
}

void initRefract() {
  // framebuffer object
  glGenFramebuffers(1, &fboRefract);
  glBindFramebuffer(GL_FRAMEBUFFER, fboRefract);

  glActiveTexture(GL_TEXTURE0 + 2);
  glGenTextures(1, &tboRefract);
  glBindTexture(GL_TEXTURE_2D, tboRefract);

  // On OSX, must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2, don't know why
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, 0,
               GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, tboRefract, 0);
  glDrawBuffer(GL_COLOR_ATTACHMENT1);
}

void initReflect() {
  // framebuffer object
  glGenFramebuffers(1, &fboReflect);
  glBindFramebuffer(GL_FRAMEBUFFER, fboReflect);

  glActiveTexture(GL_TEXTURE0 + 3);
  glGenTextures(1, &tboReflect);
  glBindTexture(GL_TEXTURE_2D, tboReflect);

  // On OSX, must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2, don't know why
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, 0,
               GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, tboReflect, 0);
  glDrawBuffer(GL_COLOR_ATTACHMENT2);
}

void drawMesh() {
  /* Pool */
  glUseProgram(shaderPool);
  glUniform1i(uniPoolTexBase, 10); // change base color
  glBindVertexArray(pool.vao);
  glDrawArrays(GL_TRIANGLES, 0, pool.faces.size() * 3);
}

void drawWater() {
  glUseProgram(shaderWater);
  uniDudvMove = myGetUniformLocation(shaderWater, "dudvMove");
  dudvMove += 0.0005f; // speed
  dudvMove = fmod(dudvMove, 1.0f);
  glUniform1f(uniDudvMove, dudvMove);

  uniCamCoord = myGetUniformLocation(shaderWater, "camCoord");
  glUniform3fv(uniCamCoord, 1, value_ptr(eyePoint));

  glBindVertexArray(vaoWater);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawSkybox() {
  glUseProgram(shaderSkybox);
  glBindVertexArray(vaoSkybox);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void initGL() {
  // Initialise GLFW
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    exit(EXIT_FAILURE);
  }

  // without setting GLFW_CONTEXT_VERSION_MAJOR and _MINOR，
  // OpenGL 1.x will be used
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  // must be used if OpenGL version >= 3.0
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                            "Dudv water simulation", NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to open GLFW window." << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetKeyCallback(window, keyCallback);

  /* Initialize GLEW */
  // without this, glGenVertexArrays will report ERROR!
  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST); // must enable depth test!!
}

void initOther() {
  // FreeImage library
  FreeImage_Initialise(true);
}

void initMatrix() {
  // common
  mat4 M, V, P;

  M = translate(mat4(1.f), vec3(0.f, 0.f, -4.f));
  V = lookAt(eyePoint, eyePoint + eyeDirection, up);
  P = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f,
                  farPlane);

  // for main window
  meshM = M;
  meshV = V;
  meshP = P;

  // for skybox
  skyboxM = M;
  oriSkyboxM = skyboxM;
  skyboxV = V;
  skyboxP = P;
}

void initShader() {
  // mesh
  shaderPool = buildShader("./shader/vsPool.glsl", "./shader/fsPool.glsl");

  // skybox
  shaderSkybox =
      buildShader("./shader/vsSkybox.glsl", "./shader/fsSkybox.glsl");

  // water
  shaderWater = buildShader("./shader/vsWater.glsl", "./shader/fsWater.glsl");
}

GLuint createTexture(GLuint texUnit, string imgDir, FREE_IMAGE_FORMAT imgType) {
  glActiveTexture(GL_TEXTURE0 + texUnit);

  FIBITMAP *texImage =
      FreeImage_ConvertTo24Bits(FreeImage_Load(imgType, imgDir.c_str()));

  GLuint tboTex;
  glGenTextures(1, &tboTex);
  glBindTexture(GL_TEXTURE_2D, tboTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(texImage),
               FreeImage_GetHeight(texImage), 0, GL_BGR, GL_UNSIGNED_BYTE,
               (void *)FreeImage_GetBits(texImage));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // release
  FreeImage_Unload(texImage);

  return tboTex;
}

void initTexture() {
  // pool base texture
  tboPoolBase = createTexture(10, "./image/stone.png", FIF_PNG);
  glActiveTexture(GL_TEXTURE0 + 10);

  // water dudv map
  tboWaterDudv = createTexture(11, "./image/dudv2.png", FIF_PNG);
  glActiveTexture(GL_TEXTURE0 + 11);

  // water normal map
  tboWaterNormal = createTexture(12, "./image/normalMap2.png", FIF_PNG);
  glActiveTexture(GL_TEXTURE0 + 12);
}

void initUniform() {
  /* Pool */
  glUseProgram(shaderPool);

  // texture
  uniPoolTexBase = myGetUniformLocation(shaderPool, "texBase");

  // transform
  uniPoolM = myGetUniformLocation(shaderPool, "M");
  uniPoolV = myGetUniformLocation(shaderPool, "V");
  uniPoolP = myGetUniformLocation(shaderPool, "P");

  glUniformMatrix4fv(uniPoolM, 1, GL_FALSE, value_ptr(meshM));
  glUniformMatrix4fv(uniPoolV, 1, GL_FALSE, value_ptr(meshV));
  glUniformMatrix4fv(uniPoolP, 1, GL_FALSE, value_ptr(meshP));

  // light
  uniLightColor = myGetUniformLocation(shaderPool, "lightColor");
  uniLightPos = myGetUniformLocation(shaderPool, "lightPos");
  uniLightPower = myGetUniformLocation(shaderPool, "lightPower");
  uniDiffuse = myGetUniformLocation(shaderPool, "diffuse");
  uniAmbient = myGetUniformLocation(shaderPool, "ambient");
  uniSpecular = myGetUniformLocation(shaderPool, "specular");

  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  glUniform3fv(uniLightPos, 1, value_ptr(lightPosition));
  glUniform1f(uniLightPower, lightPower);
  glUniform3fv(uniDiffuse, 1, value_ptr(materialDiffuse));
  glUniform3fv(uniAmbient, 1, value_ptr(materialAmbient));
  glUniform3fv(uniSpecular, 1, value_ptr(materialSpecular));

  /* Skybox */
  glUseProgram(shaderSkybox);

  // transform
  uniSkyboxM = myGetUniformLocation(shaderSkybox, "M");
  uniSkyboxV = myGetUniformLocation(shaderSkybox, "V");
  uniSkyboxP = myGetUniformLocation(shaderSkybox, "P");

  glUniformMatrix4fv(uniSkyboxM, 1, GL_FALSE, value_ptr(meshM));
  glUniformMatrix4fv(uniSkyboxV, 1, GL_FALSE, value_ptr(meshV));
  glUniformMatrix4fv(uniSkyboxP, 1, GL_FALSE, value_ptr(meshP));

  /* Water */
  glUseProgram(shaderWater);

  // transform
  uniWaterM = myGetUniformLocation(shaderWater, "M");
  uniWaterV = myGetUniformLocation(shaderWater, "V");
  uniWaterP = myGetUniformLocation(shaderWater, "P");

  glUniformMatrix4fv(uniWaterM, 1, GL_FALSE, value_ptr(meshM));
  glUniformMatrix4fv(uniWaterV, 1, GL_FALSE, value_ptr(meshV));
  glUniformMatrix4fv(uniWaterP, 1, GL_FALSE, value_ptr(meshP));

  // texture
  uniTexReflect = myGetUniformLocation(shaderWater, "texReflect");
  uniTexRefract = myGetUniformLocation(shaderWater, "texRefract");
  uniTexDudv = myGetUniformLocation(shaderWater, "texDudv");
  uniTexNormal = myGetUniformLocation(shaderWater, "texNormal");

  glUniform1i(uniTexDudv, 11);
  glUniform1i(uniTexNormal, 12);
  glUniform1i(uniTexReflect, 3);
  glUniform1i(uniTexRefract, 2);

  // light
  uniWaterLightColor = myGetUniformLocation(shaderWater, "lightColor");
  uniWaterLightPos = myGetUniformLocation(shaderWater, "lightPos");
  glUniform3fv(uniWaterLightColor, 1, value_ptr(lightColor));
  glUniform3fv(uniWaterLightPos, 1, value_ptr(lightPosition));
}
