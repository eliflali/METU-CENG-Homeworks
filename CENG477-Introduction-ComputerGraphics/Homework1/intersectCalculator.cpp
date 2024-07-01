#include "parser.h"
#include "mathFunctions.h"
#include "rayGenerator.h"
#include "intersectCalculator.h"
#include <iostream>
#include <vector>

using namespace parser;
using namespace mathFunctions;
using namespace intersectCalculator;
using namespace rayGenerator;
using namespace std;

namespace intersectCalculator
{
    /*int objectID;
        int objectType; // / can be enumerated right now 0-> null 1->sphere 2-> triangle 3-> mesh
        int faceID; //had to for meshes
        float time;
        Vec3f point; */
    bool closestHit(Ray ray, HitPoint &hitFound , const Scene &scene, int ignoreObjectId, int ignoreObjectType){
        int sphereCount = scene.spheres.size() , meshCount = scene.meshes.size() , triangleCount = scene.triangles.size();
        bool isHit = false;
        float firstHit = 3.40282E38;
        float hitTime;
        HitPoint currentHit = {0,0,0,0.0F};
        /*
        1 sphere 2 tri 3 mesh
        Vec3f m = add(ray.o,multS(scene.vertex_data[sphere.center_vertex_id-1],-1));
        LEFT THIS HERE INTENTIONALLY CHEF
        */
        for(int i = 0; i < sphereCount; i++){
            //sphere case
            Vec3f originToCenter  = subtractVectors(ray.origin,scene.vertex_data[(scene.spheres[i].center_vertex_id) -1] ); //indexing starts from 1 in vertex data but this issue might be solved
            hitTime = quadraticDelta(scene.spheres[i],originToCenter,ray); // t
            if( (0 < hitTime) && (hitTime < firstHit ) ){
                firstHit = hitTime;
                Vec3f intersectionPoint = addVectors(ray.origin,
                                                     multiplyVector(ray.direction, hitTime)); // P = o + t x d
                currentHit.objectID = i;
                currentHit.objectType = 1;
                currentHit.faceID = 0;
                currentHit.time =  hitTime;
                currentHit.point = intersectionPoint;
                isHit = true;

            }
        }

        for(int i = 0; i < triangleCount;i++){
            //triangle case
            Face current = scene.triangles[i].indices;
            Vec3f vertex0 = scene.vertex_data[current.v0_id-1], vertex1 = scene.vertex_data[current.v1_id-1], vertex2 = scene.vertex_data[current.v2_id-1];
            Vec3f column0 = subtractVectors(vertex0,vertex1), column1 = subtractVectors(vertex0, vertex2), column2 = ray.direction;
            Vec3f columnBeta = subtractVectors(vertex0,ray.origin);

            float alpha = determinant3(column0,column1,column2);
            float beta = determinant3(columnBeta,column1,column2) / alpha;
            float gamma = determinant3(column0, columnBeta, column2) / alpha;
            float time = determinant3(column0, column1, columnBeta) / alpha;

            if((beta + gamma) <= 1 && beta >= 0 && gamma >= 0 ){
                if(time > 0 &&  time < firstHit){
                    firstHit = time;
                    float alpha = 1 - beta - gamma;
                    Vec3f intersectionPoint = addVectors(addVectors(multiplyVector(vertex0, alpha),
                                                                    multiplyVector(vertex1, beta)),
                                                                    multiplyVector(vertex2, gamma)); //P = alpha*v0 + beta*v1 + gamma*v2
                    currentHit.objectID = i;
                    currentHit.objectType = 2;
                    currentHit.faceID = 0;
                    currentHit.time = time;
                    currentHit.point = intersectionPoint;
                    isHit = true;
                }
            }
        }
        vector<HitPoint> totalHits;
        for(int i = 0; i < meshCount; i++){
            //mesh case
            //for debug lets see if we hit all meshes 
            
            int faceCount = scene.meshes[i].faces.size();
            for(int j =0 ; j < faceCount;j++){
                //for each face
                Face current = scene.meshes[i].faces[j];
                Vec3f vertex0 = scene.vertex_data[current.v0_id-1], vertex1 = scene.vertex_data[current.v1_id-1], vertex2 = scene.vertex_data[current.v2_id-1];
                Vec3f column0 = subtractVectors(vertex0,vertex1), column1 = subtractVectors(vertex0, vertex2), column2 = ray.direction;
                Vec3f columnBeta = subtractVectors(vertex0,ray.origin);

                float alpha = determinant3(column0,column1,column2);
                float beta = determinant3(columnBeta,column1,column2) / alpha;
                float gamma = determinant3(column0, columnBeta, column2) / alpha;
                float time = determinant3(column0, column1, columnBeta) / alpha;

                if((beta + gamma) <= 1 && beta >= 0 && gamma >= 0 ){
                    if(time > 0 &&  time < firstHit){
                        firstHit = time;
                        float alpha = 1 - beta - gamma;
                        Vec3f intersectionPoint = addVectors(addVectors(multiplyVector(vertex0, alpha),
                                                                        multiplyVector(vertex1, beta)),
                                                                        multiplyVector(vertex2, gamma)); //P = alpha*v0 + beta*v1 + gamma*v2
                        currentHit.objectID = i;
                        currentHit.objectType = 3;
                        currentHit.faceID = j;
                        currentHit.time = time;
                        currentHit.point = intersectionPoint;
                        isHit = true;
                    }
                    
                }
            }
        }
        if(currentHit.objectID == ignoreObjectId && currentHit.objectType == ignoreObjectType)
        {
            return false;
        }
        hitFound = currentHit;
        return isHit;
    }
}