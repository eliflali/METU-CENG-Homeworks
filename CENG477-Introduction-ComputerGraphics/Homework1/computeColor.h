
#ifndef CODE_TEMPLATE_COMPUTECOLOR_H
#define CODE_TEMPLATE_COMPUTECOLOR_H

#include "parser.h"
#include "rayGenerator.h"

using namespace parser;
using namespace rayGenerator;

namespace computeColor {
    Vec3f computeWi(const Vec3f &pointOnSurface, const Vec3f &lightPosition);
    Vec3f computeIrradiance(const Vec3f &intensity, const Vec3f &pointOnSurface, const Vec3f &lightPosition);
    Vec3f computeHalfVector(const Vec3f &wi, const Vec3f &wo);
    Vec3f computeAmbient(Vec3f ambientCoeff, Vec3f ambientLight);
    Vec3f computeSpecular(Vec3f specularCoeff, float phongExponent, const PointLight &light,const Ray &ray, const Vec3f &normal, const Vec3f &wi, const Vec3f &pointOnSurface);
    Vec3f computeDiffuse(Vec3f diffuseCoeff, const PointLight &light, const Vec3f &normal, const Vec3f &pointOnSurface);
    Vec3f computePixelColor(Scene scene, Camera camera, Ray ray, int maxRecursion, int ignoreObjectID = -1, int ignoreObjectType = -1,bool isReflection = false );
    bool checkShadow(const PointLight &light, const Vec3f &pointOnSurface, const Scene &scene, const Vec3f &normal);

};


#endif //CODE_TEMPLATE_COMPUTECOLOR_H
