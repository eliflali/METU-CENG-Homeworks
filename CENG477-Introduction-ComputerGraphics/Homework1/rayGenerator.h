#ifndef CODE_TEMPLATE_RAYGENERATOR_H
#define CODE_TEMPLATE_RAYGENERATOR_H

#include "parser.h"
using namespace parser;

namespace rayGenerator
{
    typedef struct Ray
    {
        Vec3f origin;
        Vec3f direction;
        int depth;
    } ray;

    Vec3f computeM(const Camera &camera);
    Vec3f computeQ(const Vec3f &m, const Vec3f &u, const Vec3f &v, const Camera &camera);
    Vec3f computeS(const Vec3f &q, const Vec3f &u, const Vec3f &v, float su, float sv);
    Ray generateRay(const Camera &camera, int i, int j);
}

#endif //CODE_TEMPLATE_RAYGENERATOR_H
