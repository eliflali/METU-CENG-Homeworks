#include <cmath>
#include "parser.h"
#include "mathFunctions.h"
#include "rayGenerator.h"
#include <limits>

using namespace parser;

namespace mathFunctions
{
    float vectorLength(const Vec3f &vec)
    {
        float length = sqrt(pow(vec.x,2) + pow(vec.y,2) + pow(vec.z,2));
        return length;
    }

    Vec3f normalize(const Vec3f &vec)
    {
        Vec3f normalizedVector;
        float vectorLen = vectorLength(vec);

        normalizedVector.x = vec.x/vectorLen;
        normalizedVector.y = vec.y/vectorLen;
        normalizedVector.z = vec.z/vectorLen;

        return normalizedVector;
    }

    float determinant3(const Vec3f &vec1, const Vec3f &vec2, const Vec3f &vec3)
    {   
        return vec1.x * (vec2.y * vec3.z - vec2.z * vec3.y) -
           vec1.y * (vec2.x * vec3.z - vec2.z * vec3.x) +
           vec1.z * (vec2.x * vec3.y - vec2.y * vec3.x);
    }

    Vec3f crossProduct(const Vec3f &vec1, const Vec3f &vec2)
    {
        Vec3f product;
        product.x = vec1.y*vec2.z - vec1.z*vec2.y;
        product.y = vec1.z*vec2.x - vec1.x*vec2.z;
        product.z = vec1.x*vec2.y - vec1.y*vec2.x;
        return product;
    }

    float dotProduct(const Vec3f &vec1, const Vec3f &vec2)
    {
        float product;
        product = vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
        return product;
    }

    float computeDistance(const Vec3f &vec1, const Vec3f &vec2)
    {
        float distance = vectorLength(subtractVectors(vec1, vec2));
        return distance;
    }

    Vec3f addVectors(const Vec3f &vec1, const Vec3f &vec2)
    {
        Vec3f total;
        total.x = vec1.x+vec2.x;
        total.y = vec1.y+vec2.y;
        total.z = vec1.z+vec2.z;
        return total;
    }

    Vec3f subtractVectors(const Vec3f &vec1, const Vec3f &vec2)
    {
        Vec3f total;
        total.x = vec1.x-vec2.x;
        total.y = vec1.y-vec2.y;
        total.z = vec1.z-vec2.z;
        return total;
    }
    Vec3f multiplyVector(const Vec3f &vec, float val){
        Vec3f newVec  = {vec.x * val, vec.y * val , vec.z * val};
        return newVec;
    }
    /*float A = dotProd(ray.d,ray.d);
    float B = 2*dotProd(ray.d,m);
    float C = dotProd(m,m) - sphere.radius*sphere.radius;

    float delta = B*B - 4*A*C;

    if (delta < 0)
        return -1.0F;
    else
        return (-B - sqrt(delta))/(2*A);
}*/
    float quadraticDelta(Sphere sphere, const Vec3f originToCenter, rayGenerator::Ray ray) {
        float a = dotProduct(ray.direction, ray.direction);
        float b = 2 * dotProduct(ray.direction, originToCenter);
        float c = dotProduct(originToCenter, originToCenter) - (sphere.radius * sphere.radius);
        float delta = (b * b) - (4 * a * c);

        if (delta < 0) {
            return std::numeric_limits<float>::infinity();
        }
        else
        {
            float t1 = (-b - sqrt(delta)) / (2 * a);
            float t2 = (-b + sqrt(delta)) / (2 * a);

            if (t1 > 0 && t2 > 0)
            {
                return std::min(t1, t2);
            }
            else if (t1 > 0)
            {
                return t1;
            }
            else if (t2 > 0)
            {
                return t2;
            }
            else
            {
                return std::numeric_limits<float>::infinity();
            }
        }
    }

}
