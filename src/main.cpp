#include "common.h"
#include "skybox.h"
#include "water.h"
#include "pool.h"

GLFWwindow *window;
Skybox *skybox;
Water *water;
Pool *pool;
Pool *mesh;

bool saveTrigger = false;
int frameNumber = 0;

float verticalAngle = -1.98947;
float horizontalAngle = 4.70905;
float initialFoV = 45.0f;
float speed = 5.0f;
float mouseSpeed = 0.005f;
float nearPlane = 0.01f, farPlane = 2000.f;

vec3 eyePoint = vec3(-0.235574, 4.012722, -5.172266);
vec3 eyeDirection =
    vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle),
         sin(verticalAngle) * sin(horizontalAngle));
vec3 up = vec3(0.f, 1.f, 0.f);

mat4 model, view, projection;

// for reflection texture
float verticalAngleReflect;
float horizontalAngleReflect;
vec3 eyePointReflect;
mat4 reflectV;

vec3 lightPosition = vec3(-5.f, 10.f, 5.f);
vec3 lightColor = vec3(1.f, 1.f, 1.f);
float lightPower = 12.f;

void computeMatricesFromInputs();
void keyCallback(GLFWwindow *, int, int, int, int);
void initGL();
void initOther();
void initTexture();
void initMatrix();

int main(int argc, char **argv) {
  initGL();
  initOther();

  skybox = new Skybox();
  water = new Water();
  pool = new Pool("./mesh/terrain.obj");
  mesh = new Pool("./mesh/cube.obj");

  initTexture();
  initMatrix();

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
    glBindFramebuffer(GL_FRAMEBUFFER, water->fboRefract);
    // for user-defined framebuffer,
    // must clear the depth buffer before rendering to enable depth test
    glClear(GL_DEPTH_BUFFER_BIT);

    // clipping
    glEnable(GL_CLIP_DISTANCE0);
    glDisable(GL_CLIP_DISTANCE1);

    vec4 clipPlane0 = vec4(0, -1, 0, Water::WATER_Y);
    glUseProgram(pool->shader);
    glUniform4fv(pool->uniClipPlane0, 1, value_ptr(clipPlane0));

    glUseProgram(mesh->shader);
    glUniform4fv(mesh->uniClipPlane0, 1, value_ptr(clipPlane0));

    // draw scene
    skybox->draw(model, view, projection, eyePoint);

    mat4 poolM = translate(mat4(1.f), vec3(0.f, 1.5f, 0.f));
    poolM = scale(poolM, vec3(3.f, 3.f, 3.f));
    pool->draw(poolM, view, projection, eyePoint, lightColor, lightPosition, 13,
               14);

    mat4 meshM = translate(mat4(1.f), vec3(-2.f, 2.1f, -2.f));
    meshM = scale(meshM, vec3(0.25f, 0.25f, 0.25f));
    mesh->draw(meshM, view, projection, eyePoint, lightColor, lightPosition, 15,
               16);

    /* render to reflection texture */
    glBindFramebuffer(GL_FRAMEBUFFER, water->fboReflect);
    // for user-defined framebuffer,
    // must clear the depth buffer before rendering to enable depth test
    glClear(GL_DEPTH_BUFFER_BIT);

    // clipping
    glDisable(GL_CLIP_DISTANCE0);
    glEnable(GL_CLIP_DISTANCE1);

    // for reflection texture,
    // the eye point and direction are symmetric to xz-plane
    // so we must change the view matrix for the scene
    // note: plane (0, 1, 0, D) means plane y = -D, not y = D
    vec4 clipPlane1 = vec4(0.f, 1.f, 0.f, -(Water::WATER_Y) + 0.125f);
    glUseProgram(pool->shader);
    glUniform4fv(pool->uniClipPlane1, 1, value_ptr(clipPlane1));

    glUseProgram(mesh->shader);
    glUniform4fv(mesh->uniClipPlane1, 1, value_ptr(clipPlane1));

    // draw scene
    skybox->draw(model, reflectV, projection, eyePointReflect);
    pool->draw(poolM, reflectV, projection, eyePoint, lightColor, lightPosition,
               13, 14);
    mesh->draw(meshM, reflectV, projection, eyePoint, lightColor, lightPosition,
               15, 16);

    /* render to main screen */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_CLIP_DISTANCE0);
    glDisable(GL_CLIP_DISTANCE1);

    // change back to the original view matrix
    // draw scene
    skybox->draw(model, view, projection, eyePoint);
    pool->draw(poolM, view, projection, eyePoint, lightColor, lightPosition, 13,
               14);
    water->draw(model, view, projection, eyePoint, lightColor, lightPosition);
    mesh->draw(meshM, view, projection, eyePoint, lightColor, lightPosition, 15,
               16);

    // refresh frame
    glfwSwapBuffers(window);

    if (saveTrigger) {
      string dir = "./result/output";
      // zero padding
      // e.g. "output0001.bmp"
      string num = to_string(frameNumber);
      num = string(4 - num.length(), '0') + num;
      string output = dir + num + ".bmp";

      // must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2 on OSX, don't know why
      FIBITMAP *outputImage =
          FreeImage_AllocateT(FIT_UINT32, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2);
      glReadPixels(0, 0, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, GL_BGRA,
                   GL_UNSIGNED_INT_8_8_8_8_REV,
                   (GLvoid *)FreeImage_GetBits(outputImage));
      FreeImage_Save(FIF_BMP, outputImage, output.c_str(), 0);
      std::cout << output << " saved." << '\n';
      frameNumber++;
    }

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();

  delete water;
  delete pool;
  delete skybox;

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

  horizontalAngleReflect = horizontalAngle;
  verticalAngleReflect = 3.1415f - verticalAngle;

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

  float dist = 2.f * (eyePoint.y - Water::WATER_Y);
  eyePointReflect = vec3(eyePoint.x, eyePoint.y - dist, eyePoint.z);

  view = lookAt(eyePoint, eyePoint + direction, newUp);
  projection = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT,
                           nearPlane, farPlane);

  // for reflect
  reflectV =
      lookAt(eyePointReflect, eyePointReflect + directionReflect, newUpReflect);

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
  model = translate(mat4(1.f), vec3(0.f, 0.f, 0.f));
  view = lookAt(eyePoint, eyePoint + eyeDirection, up);
  projection = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT,
                           nearPlane, farPlane);
}

void initTexture() {
  pool->setTexture(pool->tboBase, 13, "./image/ground_dirt_007_basecolor.jpg",
                   FIF_JPEG);
  pool->setTexture(pool->tboNormal, 14, "./image/ground_dirt_007_normal.jpg",
                   FIF_JPEG);

  mesh->setTexture(mesh->tboBase, 15, "./image/wood_plancks_004_basecolor.jpg",
                   FIF_JPEG);
  mesh->setTexture(mesh->tboNormal, 16, "./image/wood_plancks_004_normal.jpg",
                   FIF_JPEG);
}
