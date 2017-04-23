//skybox

#include "common.h"

GLFWwindow* window;

float verticalAngle = -2.5f;
float horizontalAngle = 0.5f;
float initialFoV = 45.0f;
float speed = 5.0f;
float mouseSpeed = 0.005f;
float farPlane = 2000.f;

vec3 eyePoint = vec3( 12.f, 13.f, 1.f );
vec3 eyeDirection = vec3(
    sin(verticalAngle) * cos(horizontalAngle),
    cos(verticalAngle),
    sin(verticalAngle) * sin(horizontalAngle)
);
vec3 up = vec3( 0.f, 1.f, 0.f );

vec3 lightPosition = vec3( 3.f, 3.f, 3.f );
vec3 lightColor = vec3( 1.f, 1.f, 1.f );
float lightPower = 12.f;

vec3 materialDiffuseColor = vec3( 0.1f, 0.1f, 0.1f );
vec3 materialAmbientColor = vec3( 0.1f, 0.1f, 0.1f );
vec3 materialSpecularColor = vec3( 1.f, 1.f, 1.f );

const float SKYBOX_SIZE = 500.f;
GLfloat skyboxVertices[] = {
    // Positions
    -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,

    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,

    SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
    SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,

    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,

    -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
    SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,
    SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE,
    -SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE,

    -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
    -SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE,
    SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE
};

const float WATER_SIZE = 5.f;
const float WATER_Y = 2.2f;
GLfloat waterVertices[] = {
    -WATER_SIZE, WATER_Y, -WATER_SIZE,
    -WATER_SIZE, WATER_Y, WATER_SIZE,
    WATER_SIZE, WATER_Y, WATER_SIZE,
    WATER_SIZE, WATER_Y, WATER_SIZE,
    WATER_SIZE, WATER_Y, -WATER_SIZE,
    -WATER_SIZE, WATER_Y, -WATER_SIZE
};

GLuint vbo_skybox, obj_skybox_tex, obj_pool_tex;
GLuint vbo_model, vbo_model_normal;
GLuint vbo_water;
GLuint vao_skybox, vao_model, vao_water;
GLint uniform_model_skybox, uniform_view_skybox, uniform_projection_skybox;
GLint uniform_model_model, uniform_view_model, uniform_projection_model;
GLint uniform_model_water, uniform_view_water, uniform_projection_water;
GLint uniform_lightColor, uniform_lightPosition, uniform_lightPower, uniform_lightDirection;
GLint uniform_diffuseColor, uniform_ambientColor, uniform_specularColor;
GLint uniform_tex;
mat4 ori_model_skybox, model_skybox, view_skybox, projection_skybox;
mat4 model_water, view_water, projection_water;
mat4 model_model, view_model, projection_model;
GLuint program_skybox, program_model, program_water;

void computeMatricesFromInputs( mat4&, mat4& );
void keyCallback( GLFWwindow*, int, int, int, int );
GLuint loadCubemap(vector<string>&);

int main(int argc, char** argv){
    // Initialise GLFW
	if(!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

    //without setting GLFW_CONTEXT_VERSION_MAJOR and _MINOR，
	//OpenGL 1.x will be used
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    //must be used if OpenGL version >= 3.0
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Open a window and create its OpenGL context
	window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT,
        "GLFW window with AntTweakBar",
        NULL, NULL
    );

	if(window == NULL){
		std::cout << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, keyCallback);

    /* Initialize GLEW */
    //without this, glGenVertexArrays will report ERROR!
    glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );//must enable depth test!!

    //FreeImage library
    FreeImage_Initialise(true);

    GLuint vs, fs;
    GLint link_ok;
    /* compile and link shaders */
    //for skybox
    vs = create_shader("skybox_vs.glsl", GL_VERTEX_SHADER);
	fs = create_shader("skybox_fs.glsl", GL_FRAGMENT_SHADER);

	program_skybox = glCreateProgram();
	glAttachShader(program_skybox, vs);
	glAttachShader(program_skybox, fs);

	glLinkProgram(program_skybox);
	glGetProgramiv(program_skybox, GL_LINK_STATUS, &link_ok);

	if(link_ok == GL_FALSE){
		std::cout << "Link failed: " << "skybox shaders" << std::endl;
	}

    //for 3d models
    vs = create_shader("model_vs.glsl", GL_VERTEX_SHADER);
    fs = create_shader("model_fs.glsl", GL_FRAGMENT_SHADER);

    program_model = glCreateProgram();
    glAttachShader(program_model, vs);
    glAttachShader(program_model, fs);

    glLinkProgram(program_model);
    glGetProgramiv(program_model, GL_LINK_STATUS, &link_ok);

    if(link_ok == GL_FALSE){
        std::cout << "Link failed: " << "model shaders" << '\n';
    }

    //for water surface
    vs = create_shader("water_vs.glsl", GL_VERTEX_SHADER);
    fs = create_shader("water_fs.glsl", GL_FRAGMENT_SHADER);

    program_water = glCreateProgram();
    glAttachShader(program_water, vs);
    glAttachShader(program_water, fs);

    glLinkProgram(program_water);
	glGetProgramiv(program_water, GL_LINK_STATUS, &link_ok);

	if(link_ok == GL_FALSE){
		std::cout << "Link failed." << std::endl;
	}

    //for skybox
    glUseProgram(program_skybox);
    glGenVertexArrays( 1, &vao_skybox );
    glBindVertexArray( vao_skybox );

    //texture
    vector<string> texture_images;
    texture_images.push_back("right.png");
    texture_images.push_back("left.png");
    texture_images.push_back("bottom.png");
    texture_images.push_back("top.png");
    texture_images.push_back("back.png");
    texture_images.push_back("front.png");

    glGenTextures( 1, &obj_skybox_tex );
    glActiveTexture( 0 );
    glBindTexture(GL_TEXTURE_CUBE_MAP, obj_skybox_tex);

    for (GLuint i = 0; i < texture_images.size(); i++){
        int width, height;
        FIBITMAP* image;

        image = FreeImage_ConvertTo24Bits( FreeImage_Load(FIF_PNG, texture_images[i].c_str()) );
        //image = FreeImage_ConvertTo24Bits( image );
        width = FreeImage_GetWidth(image);
        height = FreeImage_GetHeight(image);
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height,
            0, GL_BGR, GL_UNSIGNED_BYTE,
            (void*)FreeImage_GetBits(image)
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    uniform_model_skybox = myGetUniformLocation(program_skybox, "model_skybox");
    uniform_view_skybox = myGetUniformLocation(program_skybox, "view_skybox");
    uniform_projection_skybox = myGetUniformLocation(program_skybox, "projection_skybox");

    model_skybox = translate( mat4( 1.f ), vec3( 0.f, 0.f, -4.f ) );
    ori_model_skybox = model_skybox;
    view_skybox = lookAt( eyePoint, eyePoint + eyeDirection, up );
    projection_skybox = perspective(
        initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, farPlane
    );
    glUniformMatrix4fv( uniform_model_skybox, 1, GL_FALSE, value_ptr( model_skybox ) );
    glUniformMatrix4fv( uniform_view_skybox, 1, GL_FALSE, value_ptr( view_skybox ) );
    glUniformMatrix4fv( uniform_projection_skybox, 1, GL_FALSE, value_ptr( projection_skybox ) );

    glGenBuffers( 1, &vbo_skybox );
    glBindBuffer( GL_ARRAY_BUFFER, vbo_skybox );
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat)*6*6*3,
        skyboxVertices, GL_STATIC_DRAW
    );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );

    glBindVertexArray(0);
    glUseProgram(0);

    ///////////////////////////////////////////////////////////////////////////////
    //for 3d model
    mesh_info_t mesh = load_obj( "sand_pool.obj" );

    //write vertex coordinate to array
    FACE_INFO& faces = mesh.faceInfos[0];
    int faceNumber = faces.size();

    //every face includes 3 vertices, so faceNumber*3
    //every vertex coord includes 3 components, so faceNumber*3*3
    GLfloat* vertex_coords = new GLfloat[ faceNumber*3*3 ];
    GLfloat* texture_coords = new GLfloat[ faceNumber*3*2 ];
    GLfloat* normal_coords = new GLfloat[ faceNumber*3*3 ];

    for (size_t i = 0; i < faceNumber; i++) {
        //vertex coords
        for (size_t j = 0; j < 3; j++) {
            int idxVertex = faces[i].vertexIndices[j];
            vertex_coords[ i*9+j*3+0 ] = mesh.vertexCoords[ idxVertex ].x;
            vertex_coords[ i*9+j*3+1 ] = mesh.vertexCoords[ idxVertex ].y;
            vertex_coords[ i*9+j*3+2 ] = mesh.vertexCoords[ idxVertex ].z;
        }

        //texture coords
        for (size_t j = 0; j < 3; j++) {
            int idxTexture = faces[i].textureCoordIndices[j];
            texture_coords[ i*6+j*2+0 ] = mesh.textureCoords[ idxTexture ].x;
            texture_coords[ i*6+j*2+1 ] = mesh.textureCoords[ idxTexture ].y;
        }

        //normals
        for (size_t j = 0; j < 3; j++) {
            int idxNormal = faces[i].normalIndices[j];
            normal_coords[ i*9+j*3+0 ] = mesh.vertexNormals[ idxNormal ].x;
            normal_coords[ i*9+j*3+1 ] = mesh.vertexNormals[ idxNormal ].y;
            normal_coords[ i*9+j*3+2 ] = mesh.vertexNormals[ idxNormal ].z;
        }
    }

    //buffer objects
    glUseProgram(program_model);
    glGenBuffers( 1, &vbo_model );
    glGenBuffers( 1, &vbo_model_normal );

    glGenVertexArrays( 1, &vao_model );
    glBindVertexArray( vao_model );

    //vbo for pool model
    glBindBuffer( GL_ARRAY_BUFFER, vbo_model );
    glBufferData( GL_ARRAY_BUFFER,
        sizeof( GLfloat ) * faceNumber * 3 * 3,
        vertex_coords, GL_STATIC_DRAW
    );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );

    //vbo for pool texture
    glGenBuffers(1, &obj_pool_tex);
    glBindBuffer(GL_ARRAY_BUFFER, obj_pool_tex);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * faceNumber * 3 * 2,
        texture_coords, GL_STATIC_DRAW
    );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 1 );

    //vertex normal
    glBindBuffer( GL_ARRAY_BUFFER, vbo_model_normal );
    glBufferData( GL_ARRAY_BUFFER,
        sizeof( GLfloat ) * faceNumber * 3 * 3,
        normal_coords, GL_STATIC_DRAW
    );
    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 2 );

    //texture
    FIBITMAP* poolImage = FreeImage_Load(FIF_PNG, "stone.png");

    glGenTextures(1, &obj_pool_tex);
    glBindTexture(GL_TEXTURE_2D, obj_pool_tex);
    glActiveTexture(0);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,
        FreeImage_GetWidth(poolImage), FreeImage_GetHeight(poolImage),
        0, GL_BGR, GL_UNSIGNED_BYTE,
        (void*)FreeImage_GetBits(poolImage)
    );
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR
    );

    uniform_tex = myGetUniformLocation(program_model, "tex");
    glUniform1i(uniform_tex, 0);

    uniform_model_model = myGetUniformLocation(program_model, "model_model");
    uniform_view_model = myGetUniformLocation(program_model, "view_model");
    uniform_projection_model = myGetUniformLocation(program_model, "projection_model");

    model_model = translate( mat4( 1.f ), vec3( 0.f, 0.f, -4.f ) );
    view_model = lookAt( eyePoint, eyePoint + eyeDirection, up );
    projection_model = perspective(
        initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, farPlane
    );

    glUniformMatrix4fv( uniform_model_model, 1, GL_FALSE, value_ptr( model_model ) );
    glUniformMatrix4fv( uniform_view_model, 1, GL_FALSE, value_ptr( view_model ) );
    glUniformMatrix4fv( uniform_projection_model, 1, GL_FALSE, value_ptr( projection_model ) );

    //light
    uniform_lightColor = myGetUniformLocation( program_model, "lightColor" );
    glUniform3fv( uniform_lightColor, 1, value_ptr( lightColor ) );

    uniform_lightPosition = myGetUniformLocation( program_model, "lightPosition" );
    glUniform3fv( uniform_lightPosition, 1, value_ptr( lightPosition ) );

    uniform_lightPower = myGetUniformLocation( program_model, "lightPower" );
    glUniform1f( uniform_lightPower, lightPower );

    uniform_diffuseColor = myGetUniformLocation( program_model, "diffuseColor" );
    glUniform3fv( uniform_diffuseColor, 1, value_ptr( materialDiffuseColor ) );

    uniform_ambientColor = myGetUniformLocation( program_model, "ambientColor" );
    glUniform3fv( uniform_ambientColor, 1, value_ptr( materialAmbientColor ) );

    uniform_specularColor = myGetUniformLocation( program_model, "specularColor" );
    glUniform3fv( uniform_specularColor, 1, value_ptr( materialSpecularColor ) );

    glBindVertexArray(0);
    glUseProgram(0);

    //////////////////////////////////////////////////////////////////////////////
    //for water surface
    glUseProgram(program_water);
    glGenVertexArrays( 1, &vao_water );
    glBindVertexArray( vao_water );

    glGenBuffers(1, &vbo_water);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_water);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(GLfloat)*6*3,
        waterVertices, GL_STATIC_DRAW
    );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    uniform_model_water = myGetUniformLocation(program_water, "model_water");
    uniform_view_water = myGetUniformLocation(program_water, "view_water");
    uniform_projection_water = myGetUniformLocation(program_water, "projection_water");

    model_water = translate( mat4( 1.f ), vec3( 0.f, 0.f, -4.f ) );
    view_water = lookAt( eyePoint, eyePoint + eyeDirection, up );
    projection_water = perspective(
        initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, farPlane
    );
    glUniformMatrix4fv( uniform_model_water, 1, GL_FALSE, value_ptr( model_water ) );
    glUniformMatrix4fv( uniform_view_water, 1, GL_FALSE, value_ptr( view_water ) );
    glUniformMatrix4fv( uniform_projection_water, 1, GL_FALSE, value_ptr( projection_water ) );

    glBindVertexArray(0);
    glUseProgram(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor( 97/256.f, 175/256.f, 239/256.f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        //draw skybox
        glUseProgram(program_skybox);
        glBindVertexArray(vao_skybox);
        computeMatricesFromInputs(projection_skybox, view_skybox);
        glUniformMatrix4fv( uniform_view_skybox, 1, GL_FALSE, value_ptr( view_skybox ) );
        glUniformMatrix4fv( uniform_projection_skybox, 1, GL_FALSE, value_ptr( projection_skybox ) );
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //draw 3d models
        glUseProgram(program_model);
        glBindVertexArray(vao_model);
        computeMatricesFromInputs(projection_model, view_model);
        glUniformMatrix4fv( uniform_view_model, 1, GL_FALSE, value_ptr( view_model ) );
        glUniformMatrix4fv( uniform_projection_model, 1, GL_FALSE, value_ptr( projection_model ) );
        glDrawArrays(GL_TRIANGLES, 0, faceNumber*3);

        //draw water surface
        glUseProgram(program_water);
        glBindVertexArray(vao_water);
        computeMatricesFromInputs(projection_water, view_water);
        glUniformMatrix4fv( uniform_view_water, 1, GL_FALSE, value_ptr( view_water ) );
        glUniformMatrix4fv( uniform_projection_water, 1, GL_FALSE, value_ptr( projection_water ) );
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    delete[] vertex_coords;
    delete[] texture_coords;
    delete[] normal_coords;

    glfwTerminate();

    //FreeImage library
    FreeImage_DeInitialise();

    return EXIT_SUCCESS;
}

void computeMatricesFromInputs( mat4& newProject, mat4& newView ){
	// glfwGetTime is called only once, the first time this function is called
	static float lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	float currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos( window, &xpos, &ypos );

	// Reset mouse position for next frame
	glfwSetCursorPos( window, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 );

	// Compute new orientation
    //因为事先一步固定光标在屏幕中心
    //所以 WINDOW_WIDTH/2.f - xpos 和 WINDOW_HEIGHT/2.f - ypos 成了移动量
	horizontalAngle += mouseSpeed * float( xpos - WINDOW_WIDTH/2.f );
	verticalAngle += mouseSpeed * float( - ypos + WINDOW_HEIGHT/2.f );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	vec3 direction = vec3(
		sin(verticalAngle) * cos(horizontalAngle),
		cos(verticalAngle),
		sin(verticalAngle) * sin(horizontalAngle)
	);

	// Right vector
	vec3 right = vec3(
        cos(horizontalAngle - 3.14/2.f), 0.f, sin(horizontalAngle - 3.14/2.f)
    );

	//new up vector
	vec3 newUp = cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		eyePoint += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		eyePoint -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		eyePoint += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		eyePoint -= right * deltaTime * speed;
	}

	//float FoV = initialFoV;
	newProject = perspective(initialFoV, 1.f * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, farPlane);
	// Camera matrix
	newView = lookAt(
        eyePoint,
        eyePoint + direction,
        newUp
	);

    //使 skybox 的中心永远位于 eyePoint
    //注意：GLM 的矩阵是 column major
    model_skybox[3][0] = ori_model_skybox[0][3] + eyePoint.x;
    model_skybox[3][1] = ori_model_skybox[1][3] + eyePoint.y;
    model_skybox[3][2] = ori_model_skybox[2][3] + eyePoint.z;
    glUniformMatrix4fv( uniform_model_skybox, 1, GL_FALSE,value_ptr(model_skybox));

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void keyCallback(
    GLFWwindow* keyWnd,
    int key, int scancode, int action, int mods
){
    if(action == GLFW_PRESS){
        switch (key) {
            case GLFW_KEY_ESCAPE:
            {
                glfwSetWindowShouldClose(keyWnd, GLFW_TRUE);
                break;
            }
            case GLFW_KEY_F:
            {
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                break;
            }
            case GLFW_KEY_L:
            {
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                break;
            }
            case GLFW_KEY_I:
            {
                std::cout << "eyePoint: " << to_string( eyePoint ) << '\n';
                std::cout << "verticleAngle: " << fmod(verticalAngle, 6.28f) << ", "
                    << "horizontalAngle: " << fmod(horizontalAngle, 6.28f) << endl;
                break;
            }
            default:
                break;
        }
    }
}

GLuint loadCubemap(vector<string>& faces){
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    int width, height;
    FIBITMAP* image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i = 0; i < faces.size(); i++)
    {
        image = FreeImage_Load(FIF_PNG, faces[i].c_str());
        FreeImage_ConvertTo24Bits(image);
        width = FreeImage_GetWidth(image);
        height = FreeImage_GetHeight(image);
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, image
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}
