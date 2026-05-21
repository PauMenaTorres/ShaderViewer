/*
 *  model.h
 *  (c) 2012 ViRVIG (http://www.virvig.eu)
 *  This source code is licensed under the creative commons CC BY-NC-SA 3.0
 *  license (see http://creativecommons.org/licenses/by-nc-sa/3.0/)
 *
 */

#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

// Assimp C-API headers
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Material {
  std::string name;
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess;

  std::string map_kd="";
  std::string map_bump="";
  Material();
};
#ifndef __MODEL__DEF__
extern
#endif
std::vector<Material> Materials
#ifdef __MODEL__DEF__
(1);
#else
;
#endif

typedef double Vertex;
typedef double Normal;
typedef double TexCoord;
typedef double Tangent;
typedef double Bitangent;

struct Face{
  std::vector<int> v;   // Model::load() only generates triangles, though.
  std::vector<int> n;
  int mat;
  double normalC[3];
  std::vector<int> t;
};

class Model {
 public:
  Model();
  ~Model();
  void load(std::string path);
  const std::vector<Vertex>& vertices() const {
    return _vertices;
  }
  const std::vector<Normal>& normals() const {
    return _normals;
  }
  const std::vector<Face>& faces() const {
    return _faces;
  }

  const std::vector<TexCoord>& texCoords() const{
      return _texCoords;
  }
  void dumpStats() const;
  void dumpModel() const;

  float *VBO_vertices () {
    return _VBO_vertices;
  }
  float *VBO_normals () {
    return _VBO_normals;
  }
  float *VBO_matamb () {
    return _VBO_matamb;
  }
  float *VBO_matdiff () {
    return _VBO_matdiff;
  }
  float *VBO_matspec () {
    return _VBO_matspec;
  }
  float *VBO_matshin () {
    return _VBO_matshin;
  }

  float *VBO_texCoords() {
      return _VBO_texCoords;
  }

  float *VBO_tangents() {
      return _VBO_tangents;
  }

  float *VBO_bitangents() {
      return _VBO_bitangents;
  }

  std::string textureName;
  std::string bumpName;

 private:
  std::vector<Vertex> _vertices;
  std::vector<Normal> _normals;
  std::vector<TexCoord> _texCoords;
  std::vector<Face> _faces;

  float *_VBO_vertices, *_VBO_normals;
  float *_VBO_matamb, *_VBO_matdiff, *_VBO_matspec, *_VBO_matshin, *_VBO_texCoords;
  float *_VBO_tangents, *_VBO_bitangents;
};

#endif // MODEL_H
