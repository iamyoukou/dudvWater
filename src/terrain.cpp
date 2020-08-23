#include "terrain.h"

/* Terrain class */
Terrain::Terrain(const string fileName) {
  loadObj(fileName);
  initBuffers();
  initShader();
  initUniform();
}

Terrain::~Terrain() {
  glDeleteBuffers(1, &vboVtxs);
  glDeleteBuffers(1, &vboUvs);
  glDeleteBuffers(1, &vboNormals);
  glDeleteVertexArrays(1, &vao);
}

void Terrain::initShader() {
  string dir = "./shader/";
  string vs, fs, tcs, tes;

  vs = dir + "vsTerrain.glsl";
  fs = dir + "fsTerrain.glsl";
  tcs = dir + "tcsQuad.glsl";
  tes = dir + "tesQuad.glsl";
  // tcs = "";
  // tes = "";

  shader = buildShader(vs, fs, tcs, tes);
}

void Terrain::initUniform() {
  uniModel = myGetUniformLocation(shader, "M");
  uniView = myGetUniformLocation(shader, "V");
  uniProjection = myGetUniformLocation(shader, "P");
  uniEyePoint = myGetUniformLocation(shader, "eyePoint");
  uniLightColor = myGetUniformLocation(shader, "lightColor");
  uniLightPosition = myGetUniformLocation(shader, "lightPosition");
  uniTexBase = myGetUniformLocation(shader, "texBase");
  uniTexNormal = myGetUniformLocation(shader, "texNormal");
  uniTexHeight = myGetUniformLocation(shader, "texHeight");
  uniClipPlane0 = myGetUniformLocation(shader, "clipPlane0");
  uniClipPlane1 = myGetUniformLocation(shader, "clipPlane1");
}

void Terrain::loadObj(const string fileName) {
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

      // v4/vt4/vn4
      fin >> f.v4;
      fin.ignore(1);
      fin >> f.vt4;
      fin.ignore(1);
      fin >> f.vn4;

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

      f.v4 -= 1;
      f.vt4 -= 1;
      f.vn4 -= 1;

      faces.push_back(f);
    } else {
      continue;
    }
  } // end read obj loop

  fin.close();
}

void Terrain::initBuffers() {
  // write vertex coordinate to array
  int nOfFaces = faces.size();

  // 4 vertices per face, 3 float per vertex coord, 2 float per tex coord
  GLfloat *aVtxCoords = new GLfloat[nOfFaces * 4 * 3];
  GLfloat *aUvs = new GLfloat[nOfFaces * 4 * 2];
  GLfloat *aNormals = new GLfloat[nOfFaces * 4 * 3];

  for (size_t i = 0; i < nOfFaces; i++) {
    // vertex 1
    int vtxIdx = faces[i].v1;
    aVtxCoords[i * 12 + 0] = vertices[vtxIdx].x;
    aVtxCoords[i * 12 + 1] = vertices[vtxIdx].y;
    aVtxCoords[i * 12 + 2] = vertices[vtxIdx].z;

    // normal for vertex 1
    int nmlIdx = faces[i].vn1;
    aNormals[i * 12 + 0] = faceNormals[nmlIdx].x;
    aNormals[i * 12 + 1] = faceNormals[nmlIdx].y;
    aNormals[i * 12 + 2] = faceNormals[nmlIdx].z;

    // uv for vertex 1
    int uvIdx = faces[i].vt1;
    aUvs[i * 8 + 0] = uvs[uvIdx].x;
    aUvs[i * 8 + 1] = uvs[uvIdx].y;

    // vertex 2
    vtxIdx = faces[i].v2;
    aVtxCoords[i * 12 + 3] = vertices[vtxIdx].x;
    aVtxCoords[i * 12 + 4] = vertices[vtxIdx].y;
    aVtxCoords[i * 12 + 5] = vertices[vtxIdx].z;

    // normal for vertex 2
    nmlIdx = faces[i].vn2;
    aNormals[i * 12 + 3] = faceNormals[nmlIdx].x;
    aNormals[i * 12 + 4] = faceNormals[nmlIdx].y;
    aNormals[i * 12 + 5] = faceNormals[nmlIdx].z;

    // uv for vertex 2
    uvIdx = faces[i].vt2;
    aUvs[i * 8 + 2] = uvs[uvIdx].x;
    aUvs[i * 8 + 3] = uvs[uvIdx].y;

    // vertex 3
    vtxIdx = faces[i].v3;
    aVtxCoords[i * 12 + 6] = vertices[vtxIdx].x;
    aVtxCoords[i * 12 + 7] = vertices[vtxIdx].y;
    aVtxCoords[i * 12 + 8] = vertices[vtxIdx].z;

    // normal for vertex 3
    nmlIdx = faces[i].vn3;
    aNormals[i * 12 + 6] = faceNormals[nmlIdx].x;
    aNormals[i * 12 + 7] = faceNormals[nmlIdx].y;
    aNormals[i * 12 + 8] = faceNormals[nmlIdx].z;

    // uv for vertex 3
    uvIdx = faces[i].vt3;
    aUvs[i * 8 + 4] = uvs[uvIdx].x;
    aUvs[i * 8 + 5] = uvs[uvIdx].y;

    // vertex 4
    vtxIdx = faces[i].v4;
    aVtxCoords[i * 12 + 9] = vertices[vtxIdx].x;
    aVtxCoords[i * 12 + 10] = vertices[vtxIdx].y;
    aVtxCoords[i * 12 + 11] = vertices[vtxIdx].z;

    // normal for vertex 4
    nmlIdx = faces[i].vn4;
    aNormals[i * 12 + 9] = faceNormals[nmlIdx].x;
    aNormals[i * 12 + 10] = faceNormals[nmlIdx].y;
    aNormals[i * 12 + 11] = faceNormals[nmlIdx].z;

    // uv for vertex 4
    uvIdx = faces[i].vt4;
    aUvs[i * 8 + 6] = uvs[uvIdx].x;
    aUvs[i * 8 + 7] = uvs[uvIdx].y;
  }

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // vbo for vertex
  glGenBuffers(1, &vboVtxs);
  glBindBuffer(GL_ARRAY_BUFFER, vboVtxs);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 4 * 3, aVtxCoords,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // vbo for texture
  glGenBuffers(1, &vboUvs);
  glBindBuffer(GL_ARRAY_BUFFER, vboUvs);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 4 * 2, aUvs,
               GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  // vbo for normal
  glGenBuffers(1, &vboNormals);
  glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nOfFaces * 4 * 3, aNormals,
               GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(2);

  // delete client data
  delete[] aVtxCoords;
  delete[] aUvs;
  delete[] aNormals;
}

void Terrain::setTexture(GLuint &tbo, int texUnit, const string texDir,
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

void Terrain::draw(mat4 M, mat4 V, mat4 P, vec3 eye, vec3 lightColor,
                   vec3 lightPosition, int unitBaseColor, int unitNormal,
                   int unitHeight) {
  glUseProgram(shader);

  glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(M));
  glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(V));
  glUniformMatrix4fv(uniProjection, 1, GL_FALSE, value_ptr(P));

  glUniform3fv(uniEyePoint, 1, value_ptr(eye));

  glUniform3fv(uniLightColor, 1, value_ptr(lightColor));
  glUniform3fv(uniLightPosition, 1, value_ptr(lightPosition));

  glUniform1i(uniTexBase, unitBaseColor); // change base color
  glUniform1i(uniTexNormal, unitNormal);  // change normal
  glUniform1i(uniTexHeight, unitHeight);  // change height map

  glBindVertexArray(vao);

  glDrawArrays(GL_PATCHES, 0, faces.size() * 4);
}
