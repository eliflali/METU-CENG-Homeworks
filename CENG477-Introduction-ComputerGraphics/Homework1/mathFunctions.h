#ifndef CODE_TEMPLATE_MATHFUNCTIONS_H
#define CODE_TEMPLATE_MATHFUNCTIONS_H
#include "rayGenerator.h"
#include "parser.h"
using namespace parser;
using namespace rayGenerator;
namespace mathFunctions
{
    float vectorLength(Vec3f &vec);
    Vec3f normalize(const Vec3f &vec);

    float determinant3(const Vec3f &vec1, const Vec3f &vec2, const Vec3f &vec3);
    Vec3f crossProduct(const Vec3f &vec1, const Vec3f &vec2);
    float dotProduct(const Vec3f &vec1, const Vec3f &vec2);
    float computeDistance(const Vec3f &vec1, const Vec3f &vec2);

    Vec3f addVectors(const Vec3f &vec1, const Vec3f &vec2);
    Vec3f subtractVectors(const Vec3f &vec1, const Vec3f &vec2);
    Vec3f multiplyVector(const Vec3f &vec, float val);  // not sure if needed
    float quadraticDelta(Sphere sphere, const Vec3f originToCenter, Ray ray);
}
#endif //CODE_TEMPLATE_MATHFUNCTIONS_H
