#include "common.h"

string read_file( const string filename ){
	ifstream in;
    in.open( filename.c_str() );
    stringstream ss;
    ss << in.rdbuf();
    string sOut = ss.str();
    in.close();

	return sOut;
}

GLuint create_shader( string filename, GLenum type ){
    /* read source code */
    string sTemp = read_file( filename );
    string info;
    const GLchar* source = sTemp.c_str();

    switch( type ){
        case GL_VERTEX_SHADER:
            info = "Vertex";
            break;
        case GL_FRAGMENT_SHADER:
            info = "Fragment";
            break;
    }

    if( source == NULL ){
        std::cout << info << " Shader : Can't read shader source file." << std::endl;
        return 0;
    }

    /* 编译 */
    const GLchar* sources[] = {
        source
    };
    GLuint shader = glCreateShader( type );
    glShaderSource( shader, 1, sources, NULL );
    glCompileShader( shader );

    GLint compile_ok;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compile_ok );
    if( compile_ok == GL_FALSE ){
        std::cout << info << " Shader : Fail to compile." << std::endl;
		printLog( shader );
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void printLog( GLuint& object ){
	GLint log_length = 0;
	if ( glIsShader( object ) ) {
		glGetShaderiv( object, GL_INFO_LOG_LENGTH, &log_length );
	} else if ( glIsProgram( object ) ) {
		glGetProgramiv( object, GL_INFO_LOG_LENGTH, &log_length );
	} else {
		cerr << "printlog: Not a shader or a program" << endl;
		return;
	}

	char* log = ( char* )malloc( log_length );

	if ( glIsShader( object ) )
		glGetShaderInfoLog( object, log_length, NULL, log );
	else if ( glIsProgram( object ) )
		glGetProgramInfoLog( object, log_length, NULL, log );

	cerr << log << endl;
	free( log );
}

GLint myGetUniformLocation( GLuint& prog, string name ){
    GLint location;
    location = glGetUniformLocation( prog, name.c_str() );
    if( location == -1 ){
        cerr << "Could not bind uniform : " << name << ". "
			<< "Did you set the right name? "
			<< "Or is " << name << " not used?" << endl;
    }

    return location;
}

void drawBox( vec3 min, vec3 max ){
    float lengthX, lengthY, lengthZ;
    lengthX = max.x - min.x;
    lengthY = max.y - min.y;
    lengthZ = max.z - min.z;

    //first, write vertex coordinates into vector<vec3>
    vector<vec3> vertices;
    vertices.push_back( vec3( max - vec3( lengthX, 0.f, 0.f ) ) );//vertex 0
    vertices.push_back( vec3( min + vec3( 0.f, 0.f, lengthZ ) ) );//1
    vertices.push_back( vec3( min + vec3( lengthX, 0.f, lengthZ ) ) );//2
    vertices.push_back( vec3( max ) );//3
    vertices.push_back( vec3( max - vec3( lengthX, 0.f, lengthZ ) ) );//4
    vertices.push_back( vec3( min )  );//5
    vertices.push_back( vec3( min + vec3( lengthX, 0.f, 0.f ) ) );//6
    vertices.push_back( vec3( max - vec3( 0.f, 0.f, lengthZ ) ) );//7

    //then, write vertex coordinates from vector<vec3> to array
    //8 verices, 3 GLfloat per vertex
    GLfloat* vertexArray = new GLfloat[ 8 * 3 ];
    for (size_t i = 0; i < 8; i++) {
        vec3& vtxCoord = vertices[i];
        vertexArray[ 3*i ] = vtxCoord.x;
        vertexArray[ 3*i+1 ] = vtxCoord.y;
        vertexArray[ 3*i+2 ] = vtxCoord.z;
    }

    //vertex color
    GLfloat colorArray[] = {
        1.f, 1.f, 1.f,
        1.f, 1.f, 1.f,
        1.f, 1.f, 1.f,
        1.f, 1.f, 1.f,
        1.f, 1.f, 1.f,
        1.f, 1.f, 1.f,
        1.f, 1.f, 1.f,
        1.f, 1.f, 1.f
    };

    //vertex index
    GLushort indexArray[] = {
        0, 1, 2, 3,//front face
        4, 7, 6, 5,//back
        4, 0, 3, 7,//up
        5, 6, 2, 1,//down
        0, 4, 5, 1,//left
        3, 2, 6, 7//right
    };

    //prepare buffers to draw
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    GLuint vboVertex;
    glGenBuffers( 1, &vboVertex );
    glBindBuffer( GL_ARRAY_BUFFER, vboVertex );
    glBufferData( GL_ARRAY_BUFFER,
        sizeof( GLfloat ) * 8 * 3, vertexArray,
        GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );

    GLuint vboColor;
    glGenBuffers( 1, &vboColor );
    glBindBuffer( GL_ARRAY_BUFFER, vboColor );
    glBufferData( GL_ARRAY_BUFFER,
        sizeof( colorArray ), colorArray,
        GL_STATIC_DRAW );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 1 );

    GLuint ibo;
    glGenBuffers( 1, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
        sizeof( indexArray ), indexArray, GL_STATIC_DRAW
    );

    //draw box
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    for (size_t i = 0; i < 6; i++) {
        glDrawElements( GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT,
            ( GLvoid* )( sizeof( GLushort ) * 4 * i )
        );
    }

    delete[] vertexArray;
}

//load obj with texture coords
mesh_info_t load_obj(string filename){
    mesh_info_t mesh_info;

    ifstream fin;
    fin.open( filename.c_str() );

    if( !( fin.good() ) ){
        cerr << "failed to open file : " << filename << endl;
    }

    //格式化提取数据，放入vertices
    while( fin.peek() != EOF ){//read obj loop
        string s;
        fin >> s;

        //material file name
        if( "mtllib" == s ){
            fin >> mesh_info.mtl_file;
        }
        //vertex coordinate
        else if( "v" == s ){
            vec3 vertex_coord;
            fin >> vertex_coord.x;
            fin >> vertex_coord.y;
            fin >> vertex_coord.z;
            mesh_info.vertexCoords.push_back(vertex_coord);
        }
        //vertex texture coord
        else if( "vt" == s ){
            vec2 texture_coord;
            fin >> texture_coord.x;
            fin >> texture_coord.y;
            mesh_info.textureCoords.push_back(texture_coord);
        }
        //face normal (recorded as vn in obj file)
        else if( "vn" == s ){
            vec3 vertex_normal;
            fin >> vertex_normal.x;
            fin >> vertex_normal.y;
            fin >> vertex_normal.z;
            mesh_info.vertexNormals.push_back(vertex_normal);
        }
        else if( "usemtl" == s ){
            //save usemtl
            string na;
            fin >> na;
            mesh_info.mtl_names.push_back(na);

            //face information
            FACE_INFO face_info;
            while( (fin.peek() != 'u') && (fin.peek() != EOF) ){//until next usemtl
                string ss;
                fin >> ss;

                if("s" == ss){//保留
                    string smooth;
                    fin >> smooth;
                }
                else if("f" == ss){
                    face_info_t face;

                    //first "v/vt/vn"
                    fin >> face.vertexIndices.x;
                    fin.ignore( 1 );//remove "/"
                    fin >> face.textureCoordIndices.x;
                    fin.ignore( 1 );//remove "/"
                    fin >> face.normalIndices.x;
                    //second
                    fin >> face.vertexIndices.y;
                    fin.ignore( 1 );//remove "/"
                    fin >> face.textureCoordIndices.y;
                    fin.ignore( 1 );//remove "/"
                    fin >> face.normalIndices.y;
                    //third
                    fin >> face.vertexIndices.z;
                    fin.ignore( 1 );//remove "/"
                    fin >> face.textureCoordIndices.z;
                    fin.ignore( 1 );//remove "/"
                    fin >> face.normalIndices.z;

                    //CAUTION:
                    //  v, vt, vn in "v/vt/vn" start from 1,
                    //  but indices of std::vector start from 0,
                    //  so
                    face.vertexIndices -= 1;
                    face.textureCoordIndices -= 1;
                    face.normalIndices -= 1;

                    face_info.push_back(face);
                }
            }//end of while in read usemtl
            mesh_info.faceInfos.push_back(face_info);
        }//end read usemtl
    }//end read obj loop

    fin.close();

    return mesh_info;
}
