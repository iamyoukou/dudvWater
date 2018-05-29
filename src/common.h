#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <GLFW/glfw3.h>
#include <FreeImage.h>

using namespace std;
using namespace glm;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct face_info_t {
    //every face is triangle
    ivec3 vertexIndices;
    ivec3 textureCoordIndices;
    ivec3 normalIndices;
};

typedef vector<face_info_t> FACE_INFO;

struct mesh_info_t {
    //VERTEX_INFO vertexTable;
    vector<vec3> vertexCoords;
    vector<vec2> textureCoords;
    vector<vec3> vertexNormals;
    vector<FACE_INFO> faceInfos;
    vector<string> mtl_names;
    string mtl_file;
};

string read_file( const string );
mesh_info_t load_obj(string);
GLuint create_shader( string, GLenum );
void printLog( GLuint& );
GLint myGetUniformLocation( GLuint&, string );
void drawBox( vec3, vec3 );
