#include "common.h"

std::string readFile(const std::string filename) {
  std::ifstream in;
  in.open(filename.c_str());
  std::stringstream ss;
  ss << in.rdbuf();
  std::string sOut = ss.str();
  in.close();

  return sOut;
}

// return a shader executable
GLuint buildShader(string vsDir, string fsDir) {
  GLuint vs, fs;
  GLint linkOk;
  GLuint exeShader;

  // compile
  vs = compileShader(vsDir, GL_VERTEX_SHADER);
  fs = compileShader(fsDir, GL_FRAGMENT_SHADER);

  // link
  exeShader = linkShader(vs, fs);

  return exeShader;
}

GLuint compileShader(string filename, GLenum type) {
  /* read source code */
  string sTemp = readFile(filename);
  string info;
  const GLchar *source = sTemp.c_str();

  switch (type) {
  case GL_VERTEX_SHADER:
    info = "Vertex";
    break;
  case GL_FRAGMENT_SHADER:
    info = "Fragment";
    break;
  }

  if (source == NULL) {
    std::cout << info << " Shader : Can't read shader source file."
              << std::endl;
    return 0;
  }

  const GLchar *sources[] = {source};
  GLuint objShader = glCreateShader(type);
  glShaderSource(objShader, 1, sources, NULL);
  glCompileShader(objShader);

  GLint compile_ok;
  glGetShaderiv(objShader, GL_COMPILE_STATUS, &compile_ok);
  if (compile_ok == GL_FALSE) {
    std::cout << info << " Shader : Fail to compile." << std::endl;
    printLog(objShader);
    glDeleteShader(objShader);
    return 0;
  }

  return objShader;
}

GLuint linkShader(GLuint vsObj, GLuint fsObj) {
  GLuint exe;
  GLint linkOk;

  exe = glCreateProgram();
  glAttachShader(exe, vsObj);
  glAttachShader(exe, fsObj);
  glLinkProgram(exe);

  // check result
  glGetProgramiv(exe, GL_LINK_STATUS, &linkOk);

  if (linkOk == GL_FALSE) {
    std::cout << "Failed to link shader program." << std::endl;
    printLog(exe);
    glDeleteProgram(exe);

    return 0;
  }

  return exe;
}

void printLog(GLuint &object) {
  GLint log_length = 0;
  if (glIsShader(object)) {
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  } else if (glIsProgram(object)) {
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  } else {
    cerr << "printlog: Not a shader or a program" << endl;
    return;
  }

  char *log = (char *)malloc(log_length);

  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);

  cerr << log << endl;
  free(log);
}

GLint myGetUniformLocation(GLuint &prog, string name) {
  GLint location;
  location = glGetUniformLocation(prog, name.c_str());
  if (location == -1) {
    cerr << "Could not bind uniform : " << name << ". "
         << "Did you set the right name? "
         << "Or is " << name << " not used?" << endl;
  }

  return location;
}

/* Mesh class */
Mesh::Mesh(const string fileName) {
  loadObj(fileName);
  initBuffers();
  initShader();
  initUniform();
}

Mesh::~Mesh() {
  glDeleteBuffers(1, &vboVtxs);
  glDeleteBuffers(1, &vboUvs);
  glDeleteBuffers(1, &vboNormals);
  glDeleteVertexArrays(1, &vao);
}

void Mesh::initShader() {
  shader = buildShader("./shader/vsPhong.glsl", "./shader/fsPhong.glsl");
}

void Mesh::initUniform() {
  uniModel = myGetUniformLocation(shader, "M");
  uniView = myGetUniformLocation(shader, "V");
  uniProjection = myGetUniformLocation(shader, "P");
  uniEyePoint = myGetUniformLocation(shader, "eyePoint");
  uniLightColor = myGetUniformLocation(shader, "lightColor");
  uniLightPosition = myGetUniformLocation(shader, "lightPosition");
  uniTexBase = myGetUniformLocation(shader, "texBase");
  uniTexNormal = myGetUniformLocation(shader, "texNormal");
}

void Mesh::loadObj(const string fileName) {
  std::ifstream fin;
  fin.open(fileName.c_str());

  if (!(fin.good())) {
    std::cout << "failed to open file : " << fileName << std::endl;
  }

  while (fin.peek() != EOF) { // read obj loop
    std::string s;
    fin >> s;

    // vertex coordinate
    if ("v" == s) {
      float x, y, z;
      fin >> x;
      fin >> y;
      fin >> z;
      vertices.push_back(glm::vec3(x, y, z));
    }
    // texture coordinate
    else if ("vt" == s) {
      float u, v;
      fin >> u;
      fin >> v;
      uvs.push_back(glm::vec2(u, v));
    }
    // face normal (recorded as vn in obj file)
    else if ("vn" == s) {
      float x, y, z;
      fin >> x;
      fin >> y;
      fin >> z;
      faceNormals.push_back(glm::vec3(x, y, z));
    }
    // vertices contained in face, and face normal
    else if ("f" == s) {
      Face f;

      // v1/vt1/vn1
      fin >> f.v1;
      fin.ignore(1);
      fin >> f.vt1;
      fin.ignore(1);
      fin >> f.vn1;

      // v2/vt2/vn2
      fin >> f.v2;
      fin.ignore(1);
      fin >> f.vt2;
      fin.ignore(1);
      fin >> f.vn2;

      // v3/vt3/vn3
      fin >> f.v3;
      fin.ignore(1);
      fin >> f.vt3;
      fin.ignore(1);
      fin >> f.vn3;

      // Note:
      //  v, vt, vn in "v/vt/vn" start from 1,
      //  but indices of std::vector start from 0,
      //  so we need minus 1 for all elements
      f.v1 -= 1;
      f.vt1 -= 1;
      f.vn1 -= 1;

      f.v2 -= 1;
      f.vt2 -= 1;
      f.vn2 -= 1;

      f.v3 -= 1;
      f.vt3 -= 1;
      f.vn3 -= 1;

      faces.push_back(f);
    } else {
      continue;
    }
  } // end read obj loop

  fin.close();
}

void Mesh::initBuffers() {
  // write vertex coordinate to array
  int nOfFaces = faces.size();

  // 3 vertices per face, 3 float per vertex coord, 2 float per tex coord
  GLfloat *aVtxCoords = new GLfloat[nOfFaces * 3 * 3];
  GLfloat *aUvs = new GLfloat[nOfFaces * 3 * 2];
  GLfloat *aNormals = new GLfloat[nOfFaces * 3 * 3];

  for (size_t i = 0; i < nOfFaces; i++) {
    // vertex 1
    int vtxIdx = faces[i].v1;
    aVtxCoords[i * 9 + 0] = vertices[vtxIdx].x;
    aVtxCoords[i * 9 + 1] = vertices[vtxIdx].y;
    aVtxCoords[i * 9 + 2] = vertices[vtxIdx].z;

    // normal for vertex 1
    int nmlIdx = faces[i].vn1;
    aNormals[i * 9 + 0] = faceNormals[nmlIdx].x;
    aNormals[i * 9 + 1] = faceNormals[nmlIdx].y;
    aNormals[i * 9 + 2] = faceNormals[nmlIdx].z;

    // uv for vertex 1
    int uvIdx = faces[i].vt1;
    aUvs[i * 6 + 0] = uvs[uvIdx].x;
    aUvs[i * 6 + 1] = uvs[uvIdx].y;

    // vertex 2
    vtxIdx = faces[i].v2;
    aVtxCoords[i * 9 + 3] = vertices[vtxIdx].x;
    aVtxCoords[i * 9 + 4] = vertices[vtxIdx].y;
    aVtxCoords[i * 9 + 5] = vertices[vtxIdx].z;

    // normal for vertex 2
    nmlIdx = faces[i].vn2;
    aNormals[i * 9 + 3] = faceNormals[nmlIdx].x;
    aNormals[i * 9 + 4] = faceNormals[nmlIdx].y;
    aNormals[i * 9 + 5] = faceNormals[nmlIdx].z;

    // uv for vertex 2
    uvIdx = faces[i].vt2;
    aUvs[i * 6 + 2] = uvs[uvIdx].x;
    aUvs[i * 6 + 3] = uvs[uvIdx].y;

    // vertex 3
    vtxIdx = faces[i].v3;
    aVtxCoords[i * 9 + 6] = vertices[vtxIdx].x;
    aVtxCoords[i * 9 + 7] = vertices[vtxIdx].y;
    aVtxCoords[i * 9 + 8] = vertices[vtxIdx].z;

    // normal for vertex 3
    nmlIdx = faces[i].vn3;
    aNormals[i * 9 + 6] = faceNormals[nmlIdx].x;
    aNormals[i * 9 + 7] = faceNormals[nmlIdx].y;
    aNormals[i * 9 + 8] = faceNormals[nmlIdx].z;

    // uv for vertex 3
    uvIdx = faces[i].vt3;
    aUvs[i * 6 + 4] = uvs[uvIdx].x;
    aUvs[i * 6 + 5] = uvs[uvIdx].y;
  }

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // vbo for vertex
  glGenBuffers(1, &vboVtxs);
  glBindBuffer(GL_ARRAY_BUFFER, vboVtxs);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 3, aVtxCoords,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // vbo for texture
  glGenBuffers(1, &vboUvs);
  glBindBuffer(GL_ARRAY_BUFFER, vboUvs);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 2, aUvs,
               GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // vbo for normal
  glGenBuffers(1, &vboNormals);
  glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 3 * 3, aNormals,
               GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);

  // delete client data
  delete[] aVtxCoords;
  delete[] aUvs;
  delete[] aNormals;
}

void Mesh::setTexture(GLuint &tbo, int texUnit, const string texDir,
                      FREE_IMAGE_FORMAT imgType) {
  glActiveTexture(GL_TEXTURE0 + texUnit);

  FIBITMAP *texImage =
      FreeImage_ConvertTo24Bits(FreeImage_Load(imgType, texDir.c_str()));

  glGenTextures(1, &tbo);
  glBindTexture(GL_TEXTURE_2D, tbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FreeImage_GetWidth(texImage),
               FreeImage_GetHeight(texImage), 0, GL_BGR, GL_UNSIGNED_BYTE,
               (void *)FreeImage_GetBits(texImage));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // release
  FreeImage_Unload(texImage);
}

void Mesh::draw(mat4 M, mat4 V, mat4 P, vec3 eye, vec3 lightColor,
                vec3 lightPosition, int unitBaseColor, int unitNormal) {
  glUseProgram(shader);

  glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(M));
  glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(V));
  glUniformMatrix4fv(uniProjection, 1, GL_FALSE, value_ptr(P));

  glUniform3fv(uniEyePoint, 1, value_ptr(eye));

  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  glUniform3fv(uniLightPosition, 1, value_ptr(lightPosition));

  glUniform1i(uniTexBase, unitBaseColor); // change base color
  glUniform1i(uniTexNormal, unitNormal);  // change normal

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, faces.size() * 3);
}

void Mesh::translate(glm::vec3 xyz) {
  // move each vertex with xyz
  for (size_t i = 0; i < vertices.size(); i++) {
    vertices[i] += xyz;
  }

  // update aabb
  min += xyz;
  max += xyz;
}

void Mesh::scale(glm::vec3 xyz) {
  // scale each vertex with xyz
  for (size_t i = 0; i < vertices.size(); i++) {
    vertices[i].x *= xyz.x;
    vertices[i].y *= xyz.y;
    vertices[i].z *= xyz.z;
  }

  // update aabb
  min.x *= xyz.x;
  min.y *= xyz.y;
  min.z *= xyz.z;

  max.x *= xyz.x;
  max.y *= xyz.y;
  max.z *= xyz.z;
}

// rotate mesh along x, y, z axes
// xyz specifies the rotated angle along each axis
void Mesh::rotate(glm::vec3 xyz) {}
