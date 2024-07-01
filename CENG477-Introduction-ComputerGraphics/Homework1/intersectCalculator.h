
#ifndef CODE_TEMPLATE_INTERSECTCALCULATOR_H
#define CODE_TEMPLATE_INTERSECTCALCULATOR_H

#include "parser.h"
#include "mathFunctions.h"
#include "rayGenerator.h"
using namespace parser;
using namespace rayGenerator;

namespace intersectCalculator
{
    typedef struct HitPoint
    {
        int objectID;
        int objectType; // / can be enumerated right now 0-> null 1->sphere 2-> triangle 3-> mesh
        int faceID; //had to for meshes
        float time;
        Vec3f point; //intersection point (optional)
    }hitpoint;
    
    bool closestHit(Ray ray, HitPoint &hitsFound , const Scene &scene, int ignoreObjectId = -1,int ignoreObjectType = -1);
    
}

#endif //CODE_TEMPLATE_INTERSECTCALCULATOR_H
