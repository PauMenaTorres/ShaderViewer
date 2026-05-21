/*
 *  model.cpp
 *  (c) 2012 ViRVIG (http://www.virvig.eu)
 *  This source code is licensed under the creative commons CC BY-NC-SA 3.0
 *  license (see http://creativecommons.org/licenses/by-nc-sa/3.0/)
 *
 */

#define __MODEL__DEF__ 1
#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cassert>

using namespace std;
// === Local stuff:
static void omplenormals(vector<Face> &_faces,
             vector<Vertex> const &_vertices);
static void ompleVBOs(vector<Face> &_faces,
                  vector<Vertex> const &_vertices,
                  vector<Normal> const &_normals,
                  vector<TexCoord> const &_texCoords,
              float *&_VBO_vert, float *&_VBO_norm,
              float *&_VBO_mata, float *&_VBO_matd, float *&_VBO_matsp, float *&_VBO_matsh, float *&_VBO_tex,
              float *&_VBO_tangents, float *&_VBO_bitangents);

static string modelPath("");

// ======== Constructors and Destructors =======
Model::Model() : _vertices(0), _normals(0), _faces(0) {
  _VBO_vertices = _VBO_normals = _VBO_matamb = _VBO_matdiff = _VBO_matspec = _VBO_matshin = _VBO_texCoords = _VBO_tangents = _VBO_bitangents=NULL;
}

Model::~Model() {
  if (_VBO_vertices != NULL) delete _VBO_vertices;
  if (_VBO_normals != NULL) delete _VBO_normals;
  if (_VBO_texCoords != NULL) delete _VBO_texCoords;
  if (_VBO_tangents != NULL) delete _VBO_tangents;
  if (_VBO_bitangents != NULL) delete _VBO_bitangents;
  if (_VBO_matamb != NULL) delete _VBO_matamb;
  if (_VBO_matdiff != NULL) delete _VBO_matdiff;
  if (_VBO_matspec != NULL) delete _VBO_matspec;
  if (_VBO_matshin != NULL) delete _VBO_matshin;

}

Material::Material() : name("__load_object_default_material__") {
  ambient[0] = ambient[1] = ambient[2] = 0.1; ambient[3] = 1.0;
  diffuse[0] = diffuse[1] = 0.7; diffuse[2] = 0.0; diffuse[3] = 1.0;
  specular[0] = specular[1] = specular[2] = 1.0; specular[3] = 1.0;
  shininess = 64;
}






// ========= Public methods ==========
void Model::load(std::string filename) {
  if (! _vertices.empty()) {
    // unload previous model:
    _vertices.clear();
    _normals.clear();
    _faces.clear();
    _texCoords.clear();
  }

  Materials.clear();
  Materials.push_back(Material()); // Default material at index 0

  this->textureName = "";
  this->bumpName = "";

  size_t fiPath = filename.rfind("/");
  if (fiPath == string::npos) modelPath = "";
  else modelPath = filename.substr(0, fiPath+1);

  // Import the file using Assimp's C API
  const aiScene* scene = aiImportFile(filename.c_str(),
      aiProcess_Triangulate |
      aiProcess_GenSmoothNormals |
      aiProcess_JoinIdenticalVertices);

  if (!scene) {
      cerr << "Cannot load model file " << filename << ". Error: " << aiGetErrorString() << endl;
      return;
  }

  // 1. Load materials
  for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
      const aiMaterial* mat = scene->mMaterials[i];
      Material noumat;

      // Get material name
      aiString name;
      if (aiGetMaterialString(mat, AI_MATKEY_NAME, &name) == aiReturn_SUCCESS) {
          noumat.name = name.C_Str();
      }

      // Get ambient color
      aiColor4D ambientColor;
      if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &ambientColor) == aiReturn_SUCCESS) {
          noumat.ambient[0] = ambientColor.r;
          noumat.ambient[1] = ambientColor.g;
          noumat.ambient[2] = ambientColor.b;
          noumat.ambient[3] = ambientColor.a;
      } else {
          noumat.ambient[0] = noumat.ambient[1] = noumat.ambient[2] = 0.1f;
          noumat.ambient[3] = 1.0f;
      }

      // Get diffuse color
      aiColor4D diffuseColor;
      if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor) == aiReturn_SUCCESS) {
          noumat.diffuse[0] = diffuseColor.r;
          noumat.diffuse[1] = diffuseColor.g;
          noumat.diffuse[2] = diffuseColor.b;
          noumat.diffuse[3] = diffuseColor.a;
      } else {
          noumat.diffuse[0] = noumat.diffuse[1] = 0.7f;
          noumat.diffuse[2] = 0.0f;
          noumat.diffuse[3] = 1.0f;
      }

      // Get specular color
      aiColor4D specularColor;
      if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specularColor) == aiReturn_SUCCESS) {
          noumat.specular[0] = specularColor.r;
          noumat.specular[1] = specularColor.g;
          noumat.specular[2] = specularColor.b;
          noumat.specular[3] = specularColor.a;
      } else {
          noumat.specular[0] = noumat.specular[1] = noumat.specular[2] = 1.0f;
          noumat.specular[3] = 1.0f;
      }

      // Get shininess
      unsigned int max = 1;
      float shininess = 0.0f;
      if (aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max) == aiReturn_SUCCESS) {
          noumat.shininess = shininess;
      } else {
          noumat.shininess = 64.0f;
      }

      // Get diffuse texture
      aiString path;
      if (aiGetMaterialTexture(mat, aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS) {
          noumat.map_kd = path.C_Str();
          this->textureName = modelPath + noumat.map_kd;
      }

      // Get bump/height texture
      aiString bumpPath;
      if (aiGetMaterialTexture(mat, aiTextureType_HEIGHT, 0, &bumpPath) == aiReturn_SUCCESS) {
          noumat.map_bump = bumpPath.C_Str();
          this->bumpName = modelPath + noumat.map_bump;
      } else if (aiGetMaterialTexture(mat, aiTextureType_NORMALS, 0, &bumpPath) == aiReturn_SUCCESS) {
          noumat.map_bump = bumpPath.C_Str();
          this->bumpName = modelPath + noumat.map_bump;
      }

      Materials.push_back(noumat);
  }

  // 2. Load meshes
  for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
      const aiMesh* mesh = scene->mMeshes[m];

      unsigned int vertexOffset = _vertices.size() / 3;
      unsigned int normalOffset = _normals.size() / 3;
      unsigned int texCoordOffset = _texCoords.size() / 2;

      // Append vertices
      for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
          _vertices.push_back(mesh->mVertices[i].x);
          _vertices.push_back(mesh->mVertices[i].y);
          _vertices.push_back(mesh->mVertices[i].z);
      }

      // Append normals
      if (mesh->mNormals != NULL) {
          for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
              _normals.push_back(mesh->mNormals[i].x);
              _normals.push_back(mesh->mNormals[i].y);
              _normals.push_back(mesh->mNormals[i].z);
          }
      }

      // Append texture coordinates
      if (mesh->mTextureCoords[0] != NULL) {
          for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
              _texCoords.push_back(mesh->mTextureCoords[0][i].x);
              _texCoords.push_back(mesh->mTextureCoords[0][i].y);
          }
      }

      // Append faces
      for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
          const aiFace& face = mesh->mFaces[i];
          if (face.mNumIndices == 3) {
              Face f;
              f.mat = mesh->mMaterialIndex + 1; // Maps to index in Materials

              f.v.push_back(3 * (vertexOffset + face.mIndices[0]));
              f.v.push_back(3 * (vertexOffset + face.mIndices[1]));
              f.v.push_back(3 * (vertexOffset + face.mIndices[2]));

              if (mesh->mNormals != NULL) {
                  f.n.push_back(3 * (normalOffset + face.mIndices[0]));
                  f.n.push_back(3 * (normalOffset + face.mIndices[1]));
                  f.n.push_back(3 * (normalOffset + face.mIndices[2]));
              }

              if (mesh->mTextureCoords[0] != NULL) {
                  f.t.push_back(2 * (texCoordOffset + face.mIndices[0]));
                  f.t.push_back(2 * (texCoordOffset + face.mIndices[1]));
                  f.t.push_back(2 * (texCoordOffset + face.mIndices[2]));
              }

              _faces.push_back(f);
          }
      }
  }

  // Release Assimp Scene
  aiReleaseImport(scene);

  // Compute normals for faces (if they were generated/computed)
  omplenormals(_faces, _vertices);

  // Populate OpenGL VBOs
  ompleVBOs(_faces, _vertices, _normals, _texCoords, _VBO_vertices, _VBO_normals,
            _VBO_matamb, _VBO_matdiff, _VBO_matspec, _VBO_matshin, _VBO_texCoords,
            _VBO_tangents, _VBO_bitangents);
}


static void omplenormals(vector<Face> &_faces,
             const vector<Vertex>  &_vertices) {
  for (unsigned int i = 0; i < _faces.size(); ++i) {
    double v0[3], v1[3];
    int P0 =_faces[i].v[0];
    int P1 =_faces[i].v[1];
    int P2 =_faces[i].v[2];
    for (int j = 0; j < 3; ++j) {
      v0[j] = _vertices[P1+j] - _vertices[P0+j];
      v1[j] = _vertices[P2+j] - _vertices[P1+j];
    }
    double norm = 0;
    double *normalcara = _faces[i].normalC;
    normalcara[0] = v0[1]*v1[2] - v0[2]*v1[1];
    norm += normalcara[0]*normalcara[0];
    normalcara[1] = v0[2]*v1[0] - v0[0]*v1[2];
    norm += normalcara[1]*normalcara[1];
    normalcara[2] = v0[0]*v1[1] - v0[1]*v1[0];
    norm += normalcara[2]*normalcara[2];
    for (int j = 0; j < 3; ++j) normalcara[j] /= sqrt(norm);
  }
}

static void ompleVBOs(vector<Face> &_faces,
                      const vector<Vertex> &_vertices,
                      const vector<Normal> &_normals,
                      const vector<TexCoord> &_texCoords,
                      float *&_VBO_vert, float *&_VBO_norm,
                      float *&_VBO_mata, float *&_VBO_matd, float *&_VBO_matsp, float *&_VBO_matsh, float *&_VBO_tex,
                      float *&_VBO_tan, float *&_VBO_bitan)
{
  // Creem els VBOs amb 3*3*faces.size() doubles cadascun
  _VBO_vert = new float[3*3*_faces.size()];
  _VBO_norm = new float[3*3*_faces.size()];
  _VBO_mata = new float[3*3*_faces.size()];
  _VBO_matd = new float[3*3*_faces.size()];
  _VBO_matsp = new float[3*3*_faces.size()];
  _VBO_matsh = new float[3*_faces.size()];
  _VBO_tex  =  new float[2*3*_faces.size()];
  _VBO_tan  =  new float[3*3*_faces.size()];
  _VBO_bitan  =  new float[3*3*_faces.size()];

  int index = 0;
  int idxT=0;
  for (unsigned int f = 0; f < _faces.size(); ++f) {
    Material &mat = Materials[_faces[f].mat];
    int P0=_faces[f].v[0];
    int P1=_faces[f].v[1];
    int P2=_faces[f].v[2];

    float p0x=_vertices[P0];
    float p0y=_vertices[P0+1];
    float p0z=_vertices[P0+2];

    float p1x=_vertices[P1];
    float p1y=_vertices[P1+1];
    float p1z=_vertices[P1+2];

    float p2x=_vertices[P2];
    float p2y=_vertices[P2+1];
    float p2z=_vertices[P2+2];

    float uv0x=0.0f, uv0y=0.0f;
    float uv1x=0.0f, uv1y=0.0f;
    float uv2x=0.0f, uv2y=0.0f;

    bool hasUV=(!_texCoords.empty() && _faces[f].t.size()==3);

    if (hasUV)
    {
        int T0=_faces[f].t[0];
        int T1=_faces[f].t[1];
        int T2=_faces[f].t[2];

        uv0x=_texCoords[T0];
        uv0y=_texCoords[T0+1];

        uv1x=_texCoords[T1];
        uv1y=_texCoords[T1+1];

        uv2x=_texCoords[T2];
        uv2y=_texCoords[T2+1];
    }

    float Tx=1.0f, Ty=0.0f, Tz=0.0f;
    float Bx=1.0f, By=0.0f, Bz=0.0f;

    if (hasUV)
    {
        float edge1x=p1x-p0x;
        float edge1y=p1y-p0y;
        float edge1z=p1z-p0z;

        float edge2x=p2x-p0x;
        float edge2y=p2y-p0y;
        float edge2z=p2z-p0z;

        float du1=uv1x-uv0x;
        float dv1=uv1y-uv0y;
        float du2=uv2x-uv0x;
        float dv2=uv2y-uv0y;

        float det=du1 * dv2 - dv1 * du2;
        if (fabs(det)>1e-8f)
        {
            float r=1.0f/det;
            Tx=(edge1x * dv2 - edge2x * dv1) * r;
            Ty=(edge1y * dv2 - edge2y * dv1) * r;
            Tz=(edge1z * dv2 - edge2z * dv1) * r;

            Bx=(edge2x * du1 - edge1x * du2) * r;
            By=(edge2y * du1 - edge1y * du2) * r;
            Bz=(edge2z * du1 - edge1z * du2) * r;

            float normT=sqrt(Tx*Tx+Ty*Ty+Tz*Tz);
            if (normT>1e-8f)
            {
                Tx/=normT; Ty/=normT; Tz/=normT;
            }

            float normB=sqrt(Bx*Bx+By*By+Bz*Bz);
            if (normB>1e-8f)
            {
                Bx/=normB; By/=normB; Bz/=normB;
            }
        }
    }

    for (int i = 0; i < 3; ++i) {
      if (hasUV)
      {
        int T = _faces[f].t[i];
        _VBO_tex[idxT]   = _texCoords[T];     // u
        _VBO_tex[idxT+1] = _texCoords[T+1];   // v
      } else {
        _VBO_tex[idxT]   = 0.0f;
        _VBO_tex[idxT+1] = 0.0f;
      }

      _VBO_tan[index]=Tx;
      _VBO_tan[index+1]=Ty;
      _VBO_tan[index+2]=Tz;

      _VBO_bitan[index]=Bx;
      _VBO_bitan[index+1]=By;
      _VBO_bitan[index+2]=Bz;

      int P =_faces[f].v[i];

      for (int j = 0; j < 3; ++j) {
        _VBO_vert[index+j] = _vertices[P+j];

        if (_normals.size() != 0) {
          _VBO_norm[index+j] = _normals[_faces[f].n[i]+j];
        }
        else {
          _VBO_norm[index+j] = _faces[f].normalC[j];
        }
        _VBO_mata[index+j] = mat.ambient[j];
        _VBO_matd[index+j] = mat.diffuse[j];
        _VBO_matsp[index+j] = mat.specular[j];
      }
      _VBO_matsh[index/3] = mat.shininess;
      index += 3;
      idxT += 2;
    }
  }
}



