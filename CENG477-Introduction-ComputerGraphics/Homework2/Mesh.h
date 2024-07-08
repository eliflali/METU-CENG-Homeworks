#ifndef __MESH_H__
#define __MESH_H__
#define WIREFRAME_MESH 0
#define SOLID_MESH 1
#include "Triangle.h"

class Mesh
{

public:
    int meshId, type, numberOfTransformations, numberOfTriangles; // type=0 for wireframe, type=1 for solid
    std::vector<int> transformationIds;
    std::vector<char> transformationTypes;
    std::vector<Triangle> triangles;

    Mesh();
    Mesh(int meshId, int type, int numberOfTransformations,
         std::vector<int> transformationIds,
         std::vector<char> transformationTypes,
         int numberOfTriangles,
         std::vector<Triangle> triangles);

    friend std::ostream &operator<<(std::ostream &os, const Mesh &m);
};

#endif