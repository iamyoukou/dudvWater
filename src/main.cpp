#include "common.h"

GLFWwindow *window;

bool saveTrigger = false;
int faceNumber;
int frameNumber = 0;

float verticalAngle = -2.3f;
float horizontalAngle = 5.4f;
float initialFoV = 45.0f;
float speed = 5.0f;
float mouseSpeed = 0.005f;
float farPlane = 2000.f;
float dudv_move = 0.f;

vec3 eyePoint = vec3(5.7f, 9.7f, -5.9f);
vec3 eyeDirection =
    vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
         sin(verticalAngle) * sin(horizontalAngle));
vec3 up = vec3(0.f, 1.f, 0.f);

float verticalAngle2;
vec3 eyePoint2, eyeDirection2;

vec3 lightPosition = vec3(3.f, 3.f, 3.f);
vec3 lightColor = vec3(1.f, 1.f, 1.f);
float lightPower = 12.f;

vec3 materialDiffuseColor = vec3(0.1f, 0.1f, 0.1f);
vec3 materialAmbientColor = vec3(0.1f, 0.1f, 0.1f);
vec3 materialSpecularColor = vec3(1.f, 1.f, 1.f);

const float SKYBOX_SIZE = 500.f;
GLfloat skyboxVertices[] = {
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
GLfloat waterVertices[] = {
    // coords
    -WATER_SIZE, WATER_Y, -WATER_SIZE, -WATER_SIZE, WATER_Y, WATER_SIZE,
    WATER_SIZE, WATER_Y, WATER_SIZE, WATER_SIZE, WATER_Y, WATER_SIZE,
    WATER_SIZE, WATER_Y, -WATER_SIZE, -WATER_SIZE, WATER_Y, -WATER_SIZE,
    // texture coords for dudv
    1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

GLfloat vertices_subscreen1[] = {
    // vertex coords
    -0.1f, 0.3f, -1, -0.1f, 0.9f, -1, -0.9f, 0.9f, -1, -0.9f, 0.9f, -1, -0.9f,
    0.3f, -1, -0.1f, 0.3f, -1,

    // texture coords
    1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

GLfloat vertices_subscreen2[] = {
    // vertex coords
    0.1f, 0.3f, -1, 0.9f, 0.3f, -1, 0.1f, 0.9f, -1, 0.1f, 0.9f, -1, 0.9f, 0.3f,
    -1, 0.9f, 0.9f, -1,

    // texture coords
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

GLuint vbo_skybox, obj_skybox_tex, obj_pool_tex, obj_subscreen1_tex,
    vbo_subscreen1, obj_subscreen2_tex, vbo_subscreen2;
GLuint vbo_water;
GLuint obj_reflection_tex, obj_refraction_tex, obj_dudv_tex, obj_normal_tex;
GLuint obj_depth_tex;
GLuint vao_skybox, vao_water, vao_subscreen1, vao_subscreen2;
GLuint fbo_subscreen1, fbo_subscreen2;
GLint uniform_model_skybox, uniform_view_skybox, uniform_projection_skybox;
GLint uniPoolM, uniPoolV, uniPoolP;
GLint uniform_model_water, uniform_view_water, uniform_projection_water;
GLint uniform_lightColor, uniform_lightPosition, uniform_lightPower,
    uniform_lightDirection;
GLint uniform_diffuseColor, uniform_ambientColor, uniform_specularColor;
GLint uniform_tex, uniform_tex_subscreen1, uniform_tex_subscreen2;
GLint uniform_tex_refraction, uniform_tex_reflection, uniform_tex_dudv,
    uniform_tex_normal;
GLint uniform_tex_depth;
GLint uniform_move;
GLint uniform_camera_coord;
GLint uniform_lightColor_water, uniform_lightPosition_water;
mat4 oriMainM, mainM, mainV, mainP;
mat4 ori_model_sub2, model_sub2, view_sub2, projection_sub2;
mat4 model_skybox;
GLuint program_skybox, shaderPool, program_water, program_subscreen1,
    program_subscreen2;

Mesh pool;

void computeMatricesFromInputs();
void keyCallback(GLFWwindow *, int, int, int, int);
GLuint loadCubemap(vector<string> &);
void initGL();
void initOther();
void initMatrix();
void initSkybox();
void initPool();
void initWater();
void initSubscreen1();
void initSubscreen2();
void drawSkybox(mat4 &, mat4 &, mat4 &);
void drawModels(mat4 &, mat4 &, mat4 &);
void drawWater(mat4 &, mat4 &, mat4 &);
void drawSubscreen1();
void drawSubscreen2();

int main(int argc, char **argv) {
  initGL();
  initOther();
  initMatrix();

  // initSkybox();
  initPool();
  // initWater();
  // initSubscreen1();
  // initSubscreen2();

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

    computeMatricesFromInputs();

    // glUseProgram(program_water);
    // uniform_move = myGetUniformLocation(program_water, "dudv_move");
    // dudv_move += 0.0005f; // speed
    // dudv_move = fmod(dudv_move, 1.0f);
    // glUniform1f(uniform_move, dudv_move);

    // uniform_camera_coord = myGetUniformLocation(program_water,
    // "camera_coord"); glUniform3fv(uniform_camera_coord, 1,
    // value_ptr(eyePoint)); glUseProgram(0);

    // render to fbo_subscreen1
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo_subscreen1);
    // glClearColor(171 / 256.f, 178 / 256.f, 191 / 256.f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    // glEnable(GL_CLIP_DISTANCE0);
    // glDisable(GL_CLIP_DISTANCE1);
    //
    // vec4 planeEquation0 = vec4(0, 1, 0, -2.2);
    // glUseProgram(shaderPool);
    // GLuint uniform_clipPlane0 =
    //     myGetUniformLocation(shaderPool, "clipPlane0");
    // glUniform4fv(uniform_clipPlane0, 1, value_ptr(planeEquation0));
    // glUseProgram(0);
    //
    // drawSkybox(model_skybox, mainV, mainP);
    // drawModels(mainM, mainV, mainP);

    // render to fbo_subscreen2
    // glDisable(GL_CLIP_DISTANCE0);
    // glEnable(GL_CLIP_DISTANCE1);
    //
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo_subscreen2);
    // glClearColor(171 / 256.f, 178 / 256.f, 191 / 256.f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    // glUseProgram(shaderPool);
    // vec4 planeEquation1 = vec4(0, 1, 0, -3);
    // GLuint uniform_clipPlane1 =
    //     myGetUniformLocation(shaderPool, "clipPlane1");
    // glUniform4fv(uniform_clipPlane1, 1, value_ptr(planeEquation1));
    // glUseProgram(0);
    //
    // drawSkybox(model_skybox, view_sub2, projection_sub2);
    // drawModels(model_sub2, view_sub2, projection_sub2);

    // render to main screen
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glDisable(GL_CLIP_DISTANCE0);
    // glDisable(GL_CLIP_DISTANCE1);
    // drawSkybox(model_skybox, mainV, mainP);
    // drawModels(mainM, mainV, mainP);
    // drawWater(mainM, mainV, mainP);
    // drawSubscreen1();
    // drawSubscreen2();

    glUseProgram(shaderPool);
    glBindVertexArray(pool.vao);
    glDrawArrays(GL_TRIANGLES, 0, pool.faces.size() * 3);

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

  verticalAngle2 = abs(verticalAngle) - 3.14f;

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  vec3 direction =
      vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
           sin(verticalAngle) * sin(horizontalAngle));

  vec3 direction2 =
      vec3(sin(verticalAngle2) * cos(horizontalAngle), cos(verticalAngle2),
           sin(verticalAngle2) * sin(horizontalAngle));

  // Right vector
  vec3 right = vec3(cos(horizontalAngle - 3.14 / 2.f), 0.f,
                    sin(horizontalAngle - 3.14 / 2.f));

  // new up vector
  vec3 newUp = cross(right, direction);
  vec3 newUp2 = cross(right, direction2);

  // Move forward
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    eyePoint += direction * deltaTime * speed;
  }
  // Move backward
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    eyePoint -= direction * deltaTime * speed;
  }
  // Strafe right
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    eyePoint += right * deltaTime * speed;
  }
  // Strafe left
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    eyePoint -= right * deltaTime * speed;
  }

  eyePoint2 = vec3(eyePoint.x, -eyePoint.y, eyePoint.z);

  mainV = lookAt(eyePoint, eyePoint + direction, newUp);
  mainP = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f,
                      farPlane);

  view_sub2 = lookAt(eyePoint2, eyePoint2 + direction2, newUp2);
  projection_sub2 = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT,
                                0.1f, farPlane);

  // Let the center of the skybox always at eyePoint
  // CAUTION: the matrix of GLM is column major
  model_skybox[3][0] = oriMainM[0][3] + eyePoint.x;
  model_skybox[3][1] = oriMainM[1][3] + eyePoint.y;
  model_skybox[3][2] = oriMainM[2][3] + eyePoint.z;
  glUniformMatrix4fv(uniform_model_skybox, 1, GL_FALSE,
                     value_ptr(model_skybox));

  // update for mesh
  glUseProgram(shaderPool);
  glUniformMatrix4fv(uniPoolV, 1, GL_FALSE, value_ptr(mainV));
  glUniformMatrix4fv(uniPoolP, 1, GL_FALSE, value_ptr(mainP));

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
      std::cout << "eyePoint: " << to_string(eyePoint) << '\n';
      std::cout << "verticleAngle: " << fmod(verticalAngle, 6.28f) << ", "
                << "horizontalAngle: " << fmod(horizontalAngle, 6.28f) << endl;
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

GLuint loadCubemap(vector<string> &faces) {
  GLuint textureID;
  glGenTextures(1, &textureID);
  glActiveTexture(GL_TEXTURE0);

  int width, height;
  FIBITMAP *image;

  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  for (GLuint i = 0; i < faces.size(); i++) {
    image = FreeImage_Load(FIF_PNG, faces[i].c_str());
    FreeImage_ConvertTo24Bits(image);
    width = FreeImage_GetWidth(image);
    height = FreeImage_GetHeight(image);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  return textureID;
}

// void initSkybox() {
//   // shaders
//   GLuint vs, fs;
//   GLint link_ok;
//
//   vs = create_shader("./shader/vsSkybox.glsl", GL_VERTEX_SHADER);
//   fs = create_shader("./shader/fsSkybox.glsl", GL_FRAGMENT_SHADER);
//
//   program_skybox = glCreateProgram();
//   glAttachShader(program_skybox, vs);
//   glAttachShader(program_skybox, fs);
//
//   glLinkProgram(program_skybox);
//   glGetProgramiv(program_skybox, GL_LINK_STATUS, &link_ok);
//
//   if (link_ok == GL_FALSE) {
//     std::cout << "Link failed: "
//               << "skybox shaders" << std::endl;
//   }
//
//   glUseProgram(program_skybox);
//
//   // buffers
//   glGenVertexArrays(1, &vao_skybox);
//   glBindVertexArray(vao_skybox);
//
//   // texture
//   vector<string> texture_images;
//   texture_images.push_back("./image/right.png");
//   texture_images.push_back("./image/left.png");
//   texture_images.push_back("./image/bottom.png");
//   texture_images.push_back("./image/top.png");
//   texture_images.push_back("./image/back.png");
//   texture_images.push_back("./image/front.png");
//
//   glActiveTexture(GL_TEXTURE0);
//   glGenTextures(1, &obj_skybox_tex);
//   glBindTexture(GL_TEXTURE_CUBE_MAP, obj_skybox_tex);
//
//   for (GLuint i = 0; i < texture_images.size(); i++) {
//     int width, height;
//     FIBITMAP *image;
//
//     image = FreeImage_ConvertTo24Bits(
//         FreeImage_Load(FIF_PNG, texture_images[i].c_str()));
//
//     width = FreeImage_GetWidth(image);
//     height = FreeImage_GetHeight(image);
//     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
//     height,
//                  0, GL_BGR, GL_UNSIGNED_BYTE, (void
//                  *)FreeImage_GetBits(image));
//   }
//
//   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//   uniform_model_skybox = myGetUniformLocation(program_skybox,
//   "model_skybox"); uniform_view_skybox = myGetUniformLocation(program_skybox,
//   "view_skybox"); uniform_projection_skybox =
//       myGetUniformLocation(program_skybox, "projection_skybox");
//
//   glUniformMatrix4fv(uniform_model_skybox, 1, GL_FALSE,
//   value_ptr(mainM)); glUniformMatrix4fv(uniform_view_skybox, 1,
//   GL_FALSE, value_ptr(mainV));
//   glUniformMatrix4fv(uniform_projection_skybox, 1, GL_FALSE,
//                      value_ptr(mainP));
//
//   glGenBuffers(1, &vbo_skybox);
//   glBindBuffer(GL_ARRAY_BUFFER, vbo_skybox);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 6 * 3, skyboxVertices,
//                GL_STATIC_DRAW);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//   glEnableVertexAttribArray(0);
//
//   glBindVertexArray(0);
//   glUseProgram(0);
// }

void initPool() {
  // shaders

  // build shader
  shaderPool = buildShader("./shader/vsPool.glsl", "./shader/fsPool.glsl");
  glUseProgram(shaderPool);

  pool = loadObj("./mesh/pool.obj");
  initMesh(pool);

  // texture
  FIBITMAP *poolImage = FreeImage_Load(FIF_PNG, "./image/stone.png");

  glActiveTexture(GL_TEXTURE1);
  glGenTextures(1, &obj_pool_tex);
  glBindTexture(GL_TEXTURE_2D, obj_pool_tex);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(poolImage),
               FreeImage_GetHeight(poolImage), 0, GL_BGR, GL_UNSIGNED_BYTE,
               (void *)FreeImage_GetBits(poolImage));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  uniform_tex = myGetUniformLocation(shaderPool, "texBase");
  glUniform1i(uniform_tex, 1); // the 2nd parameter must be same as GL_TEXTUREi

  uniPoolM = myGetUniformLocation(shaderPool, "M");
  uniPoolV = myGetUniformLocation(shaderPool, "V");
  uniPoolP = myGetUniformLocation(shaderPool, "P");

  glUniformMatrix4fv(uniPoolM, 1, GL_FALSE, value_ptr(mainM));
  glUniformMatrix4fv(uniPoolV, 1, GL_FALSE, value_ptr(mainV));
  glUniformMatrix4fv(uniPoolP, 1, GL_FALSE, value_ptr(mainP));

  // light
  uniform_lightColor = myGetUniformLocation(shaderPool, "lightColor");
  glUniform3fv(uniform_lightColor, 1, value_ptr(lightColor));

  uniform_lightPosition = myGetUniformLocation(shaderPool, "lightPosition");
  glUniform3fv(uniform_lightPosition, 1, value_ptr(lightPosition));

  uniform_lightPower = myGetUniformLocation(shaderPool, "lightPower");
  glUniform1f(uniform_lightPower, lightPower);

  uniform_diffuseColor = myGetUniformLocation(shaderPool, "diffuseColor");
  glUniform3fv(uniform_diffuseColor, 1, value_ptr(materialDiffuseColor));

  uniform_ambientColor = myGetUniformLocation(shaderPool, "ambientColor");
  glUniform3fv(uniform_ambientColor, 1, value_ptr(materialAmbientColor));

  uniform_specularColor = myGetUniformLocation(shaderPool, "specularColor");
  glUniform3fv(uniform_specularColor, 1, value_ptr(materialSpecularColor));

  glBindVertexArray(0);
  glUseProgram(0);

  // delete[] vertex_coords;
  // delete[] texture_coords;
  // delete[] normal_coords;
}

// void initWater() {
//   // shaders
//   GLuint vs, fs;
//   GLint link_ok;
//
//   vs = create_shader("./shader/vsWater.glsl", GL_VERTEX_SHADER);
//   fs = create_shader("./shader/fsWater.glsl", GL_FRAGMENT_SHADER);
//
//   program_water = glCreateProgram();
//   glAttachShader(program_water, vs);
//   glAttachShader(program_water, fs);
//
//   glLinkProgram(program_water);
//   glGetProgramiv(program_water, GL_LINK_STATUS, &link_ok);
//
//   if (link_ok == GL_FALSE) {
//     std::cout << "Link failed." << std::endl;
//   }
//
//   glUseProgram(program_water);
//   glGenVertexArrays(1, &vao_water);
//   glBindVertexArray(vao_water);
//
//   glGenBuffers(1, &vbo_water);
//   glBindBuffer(GL_ARRAY_BUFFER, vbo_water);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices,
//                GL_STATIC_DRAW);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//   glEnableVertexAttribArray(0);
//
//   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
//                         (GLvoid *)(sizeof(GLfloat) * 6 * 3));
//   glEnableVertexAttribArray(1);
//
//   uniform_model_water = myGetUniformLocation(program_water, "model_water");
//   uniform_view_water = myGetUniformLocation(program_water, "view_water");
//   uniform_projection_water =
//       myGetUniformLocation(program_water, "projection_water");
//
//   glUniformMatrix4fv(uniform_model_water, 1, GL_FALSE,
//   value_ptr(mainM)); glUniformMatrix4fv(uniform_view_water, 1, GL_FALSE,
//   value_ptr(mainV)); glUniformMatrix4fv(uniform_projection_water, 1,
//   GL_FALSE,
//                      value_ptr(mainP));
//
//   // dudv texture
//   FIBITMAP *dudv_image = FreeImage_Load(FIF_PNG, "./image/dudv2.png");
//
//   glActiveTexture(GL_TEXTURE4);
//   glGenTextures(1, &obj_dudv_tex);
//   glBindTexture(GL_TEXTURE_2D, obj_dudv_tex);
//
//   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(dudv_image),
//                FreeImage_GetHeight(dudv_image), 0, GL_BGR, GL_UNSIGNED_BYTE,
//                (void *)FreeImage_GetBits(dudv_image));
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//   // normal map texture
//   FIBITMAP *normal_image = FreeImage_Load(FIF_PNG, "./image/normalMap2.png");
//
//   glActiveTexture(GL_TEXTURE5);
//   glGenTextures(1, &obj_normal_tex);
//   glBindTexture(GL_TEXTURE_2D, obj_normal_tex);
//
//   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(normal_image),
//                FreeImage_GetHeight(normal_image), 0, GL_BGR,
//                GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(normal_image));
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//   uniform_tex_reflection =
//       myGetUniformLocation(program_water, "tex_reflection");
//   uniform_tex_refraction =
//       myGetUniformLocation(program_water, "tex_refraction");
//   uniform_tex_dudv = myGetUniformLocation(program_water, "tex_dudv");
//   uniform_tex_normal = myGetUniformLocation(program_water, "tex_normal");
//
//   glUniform1i(uniform_tex_dudv, 4);   // GL_TEXTURE4
//   glUniform1i(uniform_tex_normal, 5); // GL_TEXTURE5
//   glUniform1i(uniform_tex_reflection, 3);
//   glUniform1i(uniform_tex_refraction, 2);
//
//   // light
//   uniform_lightColor_water =
//       myGetUniformLocation(program_water, "lightColor_water");
//   uniform_lightPosition_water =
//       myGetUniformLocation(program_water, "lightPosition_water");
//   glUniform3fv(uniform_lightColor_water, 1, value_ptr(lightColor));
//   glUniform3fv(uniform_lightPosition_water, 1, value_ptr(lightPosition));
//
//   glBindVertexArray(0);
//   glUseProgram(0);
// }

// void initSubscreen1() {
//   // shaders
//   GLuint vs, fs;
//   GLint link_ok;
//
//   vs = create_shader("./shader/vsSubscreen1.glsl", GL_VERTEX_SHADER);
//   fs = create_shader("./shader/fsSubscreen1.glsl", GL_FRAGMENT_SHADER);
//
//   program_subscreen1 = glCreateProgram();
//   glAttachShader(program_subscreen1, vs);
//   glAttachShader(program_subscreen1, fs);
//
//   glLinkProgram(program_subscreen1);
//   glGetProgramiv(program_subscreen1, GL_LINK_STATUS, &link_ok);
//
//   if (link_ok == GL_FALSE) {
//     std::cout << "Link failed." << std::endl;
//   }
//
//   glUseProgram(program_subscreen1);
//
//   // buffers
//   glGenVertexArrays(1, &vao_subscreen1);
//   glBindVertexArray(vao_subscreen1);
//
//   glGenBuffers(1, &vbo_subscreen1);
//   glBindBuffer(GL_ARRAY_BUFFER, vbo_subscreen1);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_subscreen1),
//                vertices_subscreen1, GL_STATIC_DRAW);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//   glEnableVertexAttribArray(0);
//
//   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
//                         (GLvoid *)(sizeof(GLfloat) * 6 * 3));
//   glEnableVertexAttribArray(1);
//
//   //
//   glGenFramebuffers(1, &fbo_subscreen1);
//   glBindFramebuffer(GL_FRAMEBUFFER, fbo_subscreen1);
//
//   glActiveTexture(GL_TEXTURE2);
//   glGenTextures(1, &obj_subscreen1_tex);
//   glBindTexture(GL_TEXTURE_2D, obj_subscreen1_tex);
//
//   // On OSX, must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2, don't know why
//   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2,
//   0,
//                GL_RGB, GL_UNSIGNED_BYTE, 0);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//   glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
//   obj_subscreen1_tex,
//                        0);
//   glDrawBuffer(GL_COLOR_ATTACHMENT1);
//
//   uniform_tex_subscreen1 =
//       myGetUniformLocation(program_subscreen1, "tex_subscreen1");
//   glUniform1i(uniform_tex_subscreen1, 2);
//
//   glBindVertexArray(0);
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);
//   glUseProgram(0);
// }

// void initSubscreen2() {
//   // shaders
//   GLuint vs, fs;
//   GLint link_ok;
//
//   vs = create_shader("./shader/vsSubscreen2.glsl", GL_VERTEX_SHADER);
//   fs = create_shader("./shader/fsSubscreen2.glsl", GL_FRAGMENT_SHADER);
//
//   program_subscreen2 = glCreateProgram();
//   glAttachShader(program_subscreen2, vs);
//   glAttachShader(program_subscreen2, fs);
//
//   glLinkProgram(program_subscreen2);
//   glGetProgramiv(program_subscreen2, GL_LINK_STATUS, &link_ok);
//
//   if (link_ok == GL_FALSE) {
//     std::cout << "Link failed." << std::endl;
//   }
//
//   glUseProgram(program_subscreen2);
//
//   // buffers
//   glGenVertexArrays(1, &vao_subscreen2);
//   glBindVertexArray(vao_subscreen2);
//
//   glGenBuffers(1, &vbo_subscreen2);
//   glBindBuffer(GL_ARRAY_BUFFER, vbo_subscreen2);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_subscreen2),
//                vertices_subscreen2, GL_STATIC_DRAW);
//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//   glEnableVertexAttribArray(0);
//
//   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
//                         (GLvoid *)(sizeof(GLfloat) * 6 * 3));
//   glEnableVertexAttribArray(1);
//
//   //
//   glGenFramebuffers(1, &fbo_subscreen2);
//   glBindFramebuffer(GL_FRAMEBUFFER, fbo_subscreen2);
//
//   glActiveTexture(GL_TEXTURE3);
//   glGenTextures(1, &obj_subscreen2_tex);
//   glBindTexture(GL_TEXTURE_2D, obj_subscreen2_tex);
//
//   // On OSX, must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2, don't know why
//   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2,
//   0,
//                GL_RGB, GL_UNSIGNED_BYTE, 0);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//   glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
//   obj_subscreen2_tex,
//                        0);
//   glDrawBuffer(GL_COLOR_ATTACHMENT2);
//
//   uniform_tex_subscreen2 =
//       myGetUniformLocation(program_subscreen2, "tex_subscreen2");
//   glUniform1i(uniform_tex_subscreen2, 3);
//
//   glBindVertexArray(0);
//   glBindFramebuffer(GL_FRAMEBUFFER, 0);
//   glUseProgram(0);
// }

void drawSkybox(mat4 &M, mat4 &V, mat4 &P) {
  glUseProgram(program_skybox);
  glBindVertexArray(vao_skybox);
  glUniformMatrix4fv(uniform_model_skybox, 1, GL_FALSE, value_ptr(M));
  glUniformMatrix4fv(uniform_view_skybox, 1, GL_FALSE, value_ptr(V));
  glUniformMatrix4fv(uniform_projection_skybox, 1, GL_FALSE, value_ptr(P));
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawModels(mat4 &M, mat4 &V, mat4 &P) {
  glUseProgram(shaderPool);
  glBindVertexArray(pool.vao);
  glUniformMatrix4fv(uniPoolV, 1, GL_FALSE, value_ptr(V));
  glUniformMatrix4fv(uniPoolP, 1, GL_FALSE, value_ptr(P));
  glDrawArrays(GL_TRIANGLES, 0, faceNumber * 3);
}

void drawWater(mat4 &M, mat4 &V, mat4 &P) {
  glUseProgram(program_water);
  glBindVertexArray(vao_water);
  glUniformMatrix4fv(uniform_view_water, 1, GL_FALSE, value_ptr(V));
  glUniformMatrix4fv(uniform_projection_water, 1, GL_FALSE, value_ptr(P));
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawSubscreen1() {
  glUseProgram(program_subscreen1);
  glBindVertexArray(vao_subscreen1);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawSubscreen2() {
  glUseProgram(program_subscreen2);
  glBindVertexArray(vao_subscreen2);
  glDrawArrays(GL_TRIANGLES, 0, 6);
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
  mainM = translate(mat4(1.f), vec3(0.f, 0.f, 0.f));
  oriMainM = mainM;
  mainV = lookAt(eyePoint, eyePoint + eyeDirection, up);
  mainP = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f,
                      farPlane);

  model_sub2 = mainM;
  ori_model_sub2 = mainM;
  view_sub2 = lookAt(eyePoint2, eyePoint2 + eyeDirection2, up);
  projection_sub2 = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT,
                                0.01f, farPlane);
}
