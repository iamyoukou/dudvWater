#include <pool.h>

Pool::Pool(const string fileName) {
  loadObj(fileName);
  initBuffers();
  initShader();
  initUniform();
}

Pool::~Pool() {
  glDeleteBuffers(1, &vboVtxs);
  glDeleteBuffers(1, &vboUvs);
  glDeleteBuffers(1, &vboNormals);
  glDeleteVertexArrays(1, &vao);
}

void Pool::initShader() {
  shader = buildShader("./shader/vsPool.glsl", "./shader/fsPool.glsl");
}

void Pool::initUniform() {
  uniModel = myGetUniformLocation(shader, "M");
  uniView = myGetUniformLocation(shader, "V");
  uniProjection = myGetUniformLocation(shader, "P");
  uniEyePoint = myGetUniformLocation(shader, "eyePoint");
  uniLightColor = myGetUniformLocation(shader, "lightColor");
  uniLightPosition = myGetUniformLocation(shader, "lightPosition");
  uniTexBase = myGetUniformLocation(shader, "texBase");
  uniTexNormal = myGetUniformLocation(shader, "texNormal");
  uniClipPlane0 = myGetUniformLocation(shader, "clipPlane0");
  uniClipPlane1 = myGetUniformLocation(shader, "clipPlane1");
}

void Pool::loadObj(const string fileName) {
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

void Pool::initBuffers() {
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

void Pool::setTexture(GLuint &tbo, int texUnit, const string texDir,
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

void Pool::draw(mat4 M, mat4 V, mat4 P, vec3 eye, vec3 lightColor,
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
