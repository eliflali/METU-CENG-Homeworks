#include <vector>
#include <iomanip>
#include "Mesh.h"
#include <ostream>

Mesh::Mesh() {}

Mesh::Mesh(int meshId, int type, int numberOfTransformations,
           std::vector<int> transformationIds,
           std::vector<char> transformationTypes,
           int numberOfTriangles,
           std::vector<Triangle> triangles)
{
    this->meshId = meshId;
    this->type = type;
    this->numberOfTransformations = numberOfTransformations;
    this->numberOfTriangles = numberOfTriangles;
    this->transformationIds = transformationIds;
    this->transformationTypes = transformationTypes;
    this->triangles = triangles;
}

std::ostream &operator<<(std::ostream &os, const Mesh &m)
{
    os << "Mesh " << m.meshId;

    if (m.type == 0)
    {
        os << " wireframe(0) with ";
    }
    else
    {
        os << " solid(1) with ";
    }

    os << std::fixed << std::setprecision(3) << m.numberOfTransformations << " transformations and " << m.numberOfTriangles << " triangles"
       << std::endl
       << "\tTriangles are:" << std::endl
       << std::fixed << std::setprecision(0);

    for (int i = 0; i < m.triangles.size(); i++)
    {
        os << "\t\t" << m.triangles[i].vertexIds[0] << " " << m.triangles[i].vertexIds[1] << " " << m.triangles[i].vertexIds[2] << std::endl;
    }

    return os;
}
