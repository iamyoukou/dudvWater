#include "common.h"
#include "skybox.h"
#include "water.h"

GLFWwindow *mainWindow;

bool saveTrigger = false;
int frameNumber = 0;

// ================================================
// Camera settings
// ================================================
float verticalAngle = -1.85176;
float horizontalAngle = 3.02591;
float initialFoV = 45.0f;
float speed = 5.0f;
float mouseSpeed = 0.005f;
float nearPlane = 0.01f, farPlane = 2000.f;
vec3 eyePoint = vec3(-0.538072, 4.445531, 14.041491);
vec3 eyeDirection =
    vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle), sin(verticalAngle) * sin(horizontalAngle));
vec3 up = vec3(0.f, 1.f, 0.f);

// Common transformation matrix
mat4 model, view, projection;

// For reflection texture
float verticalAngleReflect;
float horizontalAngleReflect;
vec3 eyePointReflect;
mat4 reflectV;

// ================================================
// 3D models
// ================================================
Skybox *skybox;
Water *water;
Mesh *name;
Mesh *scene;

// ================================================
// Lighting
// ================================================
vec3 lightPosition = vec3(-5.f, 10.f, 5.f);
vec3 lightColor = vec3(1.f, 1.f, 1.f);
float lightPower = 12.f;

// ================================================
// Function declarations
// ================================================
void computeMatricesFromInputs();
void keyCallback(GLFWwindow *, int, int, int, int);
void init();
void initGL();
void initOther();
void initMatrix();
void initMesh();

// ================================================
// Main function
// ================================================
int main(int argc, char **argv)
{
    // Initializations
    init();

    // A rough way to solve cursor position initialization problem
    // Must call glfwPollEvents once to activate glfwSetCursorPos
    // This is a glfw mechanism problem
    glfwPollEvents();
    glfwSetCursorPos(mainWindow, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    /* Loop until the user closes the mainWindow */
    while (!glfwWindowShouldClose(mainWindow))
    {
        // Clear frame
        glClearColor(97 / 256.f, 175 / 256.f, 239 / 256.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // View control
        computeMatricesFromInputs();

        // -------------------------------------------
        // Render to refraction texture
        // -------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, water->fboRefract);

        // For user-defined framebuffer,
        // must clear the depth buffer before rendering to enable depth test
        glClear(GL_DEPTH_BUFFER_BIT);

        // Config clipping planes
        glEnable(GL_CLIP_DISTANCE0);
        glDisable(GL_CLIP_DISTANCE1);

        vec4 clipPlane0 = vec4(0, -1, 0, Water::WATER_Y);

        glUseProgram(name->shader);
        glUniform4fv(name->uniClipPlane0, 1, value_ptr(clipPlane0));

        glUseProgram(scene->shader);
        glUniform4fv(scene->uniClipPlane0, 1, value_ptr(clipPlane0));

        // Draw scene
        skybox->draw(model, view, projection, eyePoint);

        mat4 nameM = translate(mat4(1.f), vec3(7.f, 2.3f, 14.f));
        nameM = scale(nameM, vec3(0.5f, 0.5f, 0.5f));
        nameM = rotate(nameM, 3.14f / 2.f, vec3(1.f, 0.f, 0.f));
        nameM = rotate(nameM, 3.14f / 2.f, vec3(0.f, 0.f, 1.f));
        name->draw(nameM, view, projection, eyePoint, lightColor, lightPosition, 15, 16);

        mat4 sceneM = translate(mat4(1.f), vec3(15.f, 1.5f, 12.f));
        scene->draw(sceneM, view, projection, eyePoint, lightColor, lightPosition, 15, 16);

        // -------------------------------------------
        // Render to reflection texture
        // -------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, water->fboReflect);

        // For user-defined framebuffer,
        // must clear the depth buffer before rendering to enable depth test
        glClear(GL_DEPTH_BUFFER_BIT);

        // Config clipping planes
        glDisable(GL_CLIP_DISTANCE0);
        glEnable(GL_CLIP_DISTANCE1);

        // For reflection texture,
        // the eye point and direction are symmetric to xz-plane
        // So we must change the view matrix for the scene
        // Note: plane (0, 1, 0, D) means plane y = -D, not y = D
        vec4 clipPlane1 = vec4(0.f, 1.f, 0.f, -Water::WATER_Y + 0.125f);

        glUseProgram(name->shader);
        glUniform4fv(name->uniClipPlane1, 1, value_ptr(clipPlane1));

        glUseProgram(scene->shader);
        glUniform4fv(scene->uniClipPlane1, 1, value_ptr(clipPlane1));

        // Draw scene
        skybox->draw(model, reflectV, projection, eyePointReflect);

        // When looking from underwater to sky,
        // the back faces of an object may be seen
        // By default, back faces are culled by OpenGL
        // This results in artifacts
        // Therefore, only disable culling face when drawing objects.
        glDisable(GL_CULL_FACE);
        name->draw(nameM, reflectV, projection, eyePoint, lightColor, lightPosition, 15, 16);
        scene->draw(sceneM, reflectV, projection, eyePoint, lightColor, lightPosition, 15, 16);
        glEnable(GL_CULL_FACE);

        // ------------------------------------------------
        // Render to main screen
        // - Must change back to the original view matrix
        // ------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Config clipping planes
        glDisable(GL_CLIP_DISTANCE0);
        glDisable(GL_CLIP_DISTANCE1);

        // Draw scene
        skybox->draw(model, view, projection, eyePoint);
        name->draw(nameM, view, projection, eyePoint, lightColor, lightPosition, 15, 16);
        scene->draw(sceneM, view, projection, eyePoint, lightColor, lightPosition, 15, 16);

        // Water surface tiling
        Water::dudvMove += 0.0005f;
        Water::dudvMove = fmod(Water::dudvMove, 1.0f);

        for (size_t i = 0; i < 15; i++)
        {
            for (size_t j = 0; j < 15; j++)
            {
                mat4 tempM = translate(mat4(1.0), vec3(2.0f * i, 0, 2.0f * j));
                water->draw(tempM, view, projection, eyePoint, lightColor, lightPosition);
            }
        }

        // Update frame
        glfwSwapBuffers(mainWindow);

        // -------------------------------------------
        // (Option) Save frame
        // -------------------------------------------
        if (saveTrigger)
        {
            string dir = "./result/output";

            // Zero padding
            // e.g. "output0001.bmp"
            string num = to_string(frameNumber);
            num = string(4 - num.length(), '0') + num;
            string output = dir + num + ".bmp";

            // Must use WINDOW_WIDTH * 2 and WINDOW_HEIGHT * 2 on macOS, don't know why
            FIBITMAP *outputImage = FreeImage_AllocateT(FIT_UINT32, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2);
            glReadPixels(0, 0, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
                         (GLvoid *)FreeImage_GetBits(outputImage));
            FreeImage_Save(FIF_BMP, outputImage, output.c_str(), 0);
            std::cout << output << " saved." << '\n';
            frameNumber++;
        }

        // Handle events
        glfwPollEvents();
    }

    // Release resources
    glfwTerminate();
    delete water;
    delete skybox;
    delete name;
    delete scene;
    FreeImage_DeInitialise();

    return EXIT_SUCCESS;
}

// =======================================================
// Recompute transformation matrices from user inputs
// =======================================================
void computeMatricesFromInputs()
{
    // glfwGetTime is called only once, the first time this function is called
    static float lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    float currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(mainWindow, &xpos, &ypos);

    // Reset mouse position for next frame
    glfwSetCursorPos(mainWindow, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    // Compute new orientation
    // As the cursor is put at the center of the screen,
    // (WINDOW_WIDTH/2.f - xpos) and (WINDOW_HEIGHT/2.f - ypos) are offsets
    horizontalAngle += mouseSpeed * float(xpos - WINDOW_WIDTH / 2.f);
    verticalAngle += mouseSpeed * float(-ypos + WINDOW_HEIGHT / 2.f);

    horizontalAngleReflect = horizontalAngle;
    verticalAngleReflect = 3.1415f - verticalAngle;

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    vec3 direction =
        vec3(sin(verticalAngle) * cos(horizontalAngle), cos(verticalAngle), sin(verticalAngle) * sin(horizontalAngle));

    vec3 directionReflect = vec3(sin(verticalAngleReflect) * cos(horizontalAngleReflect), cos(verticalAngleReflect),
                                 sin(verticalAngleReflect) * sin(horizontalAngleReflect));

    // Right vector
    vec3 right = vec3(cos(horizontalAngle - 3.14 / 2.f), 0.f, sin(horizontalAngle - 3.14 / 2.f));

    vec3 rightReflect = vec3(cos(horizontalAngleReflect - 3.14 / 2.f), 0.f, sin(horizontalAngleReflect - 3.14 / 2.f));

    // New up vector
    vec3 newUp = cross(right, direction);
    vec3 newUpReflect = cross(rightReflect, directionReflect);

    // Move forward
    if (glfwGetKey(mainWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        eyePoint += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey(mainWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        eyePoint -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey(mainWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        eyePoint += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey(mainWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        eyePoint -= right * deltaTime * speed;
    }

    // Update eye point for reflection texture
    float dist = 2.f * (eyePoint.y - Water::WATER_Y);
    eyePointReflect = vec3(eyePoint.x, eyePoint.y - dist, eyePoint.z);

    // Update common transformation matrices
    view = lookAt(eyePoint, eyePoint + direction, newUp);
    projection = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, nearPlane, farPlane);

    // Update transformation matrices for reflection texture
    reflectV = lookAt(eyePointReflect, eyePointReflect + directionReflect, newUpReflect);

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}

// ===================================================================
// Keyboard callback function
// - GLFW keyboard callback reference:
//   https://www.glfw.org/docs/3.3/input_guide.html#input_keyboard
// ===================================================================
void keyCallback(GLFWwindow *keyWnd, int key, int scancode, int action, int mods)
{
    // Key press event
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            // Esc: close window
            case GLFW_KEY_ESCAPE:
            {
                glfwSetWindowShouldClose(keyWnd, GLFW_TRUE);
                break;
            }
            // F: polygon fill mode
            case GLFW_KEY_F:
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            }
            // L: polygon line mode
            case GLFW_KEY_L:
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            }
            // I: eye point information
            case GLFW_KEY_I:
            {
                std::cout << "eyePoint: " << to_string(eyePoint) << '\n';
                std::cout << "verticleAngle: " << fmod(verticalAngle, 6.28f) << ", "
                          << "horizontalAngle: " << fmod(horizontalAngle, 6.28f) << endl;

                break;
            }
            // Y: Save trigger on/off
            case GLFW_KEY_Y:
            {
                saveTrigger = !saveTrigger;
                frameNumber = 0;
                break;
            }
            default:
                break;
        }
    }
}

// ================================================
// Initializatize everything
// ================================================
void init()
{
    // OpenGL contexts
    initGL();

    // Third-party libraries
    initOther();

    // 3D models
    initMesh();

    // Transformation matrices
    initMatrix();
}

// ===================================================================
// Initialize OpenGL context
// ===================================================================
void initGL()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        exit(EXIT_FAILURE);
    }

    // Without setting GLFW_CONTEXT_VERSION_MAJOR and _MINOR，
    // OpenGL 1.x will be used
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Must apply the following settings if OpenGL version >= 3.0 is used
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create mainWindow and its OpenGL context
    mainWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dudv water simulation", NULL, NULL);
    if (mainWindow == NULL)
    {
        std::cout << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(mainWindow);

    // Input settings
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(mainWindow, keyCallback);

    // Without this, glGenVertexArrays will report ERROR!
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Face culling and depth test
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

// ================================================
// Initialize third-party libraries
// ================================================
void initOther()
{
    // FreeImage
    FreeImage_Initialise(true);
}

// ================================================
// Initialize transformation matrices
// ================================================
void initMatrix()
{
    model = translate(mat4(1.f), vec3(0.f, 0.f, 0.f));
    view = lookAt(eyePoint, eyePoint + eyeDirection, up);
    projection = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, nearPlane, farPlane);
}

// ================================================
// Initialize mesh
// ================================================
void initMesh()
{
    skybox = new Skybox();
    water = new Water();
    name = new Mesh("./mesh/name.obj", true);
    scene = new Mesh("./mesh/scene.obj", true);
}
