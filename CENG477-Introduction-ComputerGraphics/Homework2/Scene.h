#ifndef _SCENE_H_
#define _SCENE_H_
#include "Vec3.h"
#include "Vec4.h"
#include "Color.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Camera.h"
#include "Mesh.h"
#include "Matrix4.h"
#include "Helpers.h"
#include <vector>
#include <string>


class TriangleOfModel;
class Model;
class Scene;

class Model{
    public:
        std::vector<TriangleOfModel> triangles; //şef bu avelce oldu bunu sileriz en sonda
        Matrix4 modelingTransform,cameraModel,orthographicModel,perspectiveModel,viewportModel;
        int meshId,type,numberOfTriangles;
        Model(){}
        Model(Mesh *mesh,Scene *scene){
            this->meshId = mesh->meshId;
            this->type = mesh->type;
            this->numberOfTriangles = mesh->numberOfTriangles;
            this->cameraModel = getIdentityMatrix();
            this->orthographicModel = getIdentityMatrix();
            this->perspectiveModel = getIdentityMatrix();
            this->viewportModel = getIdentityMatrix();
            this->modelingTransform = getIdentityMatrix();
        }

};
class Scene
{
    public:
        Color backgroundColor;
        bool cullingEnabled;

        std::vector<std::vector<Color> > image;
        std::vector<std::vector<double> > depth;
        std::vector<Camera *> cameras;
        std::vector<Vec3 *> vertices;
        std::vector<Color *> colorsOfVertices;
        std::vector<Scaling *> scalings;
        std::vector<Rotation *> rotations;
        std::vector<Translation *> translations;
        std::vector<Mesh *> meshes;

        Scene(const char *xmlPath);

        void assignColorToPixel(int i, int j, Color c);
        void initializeImage(Camera *camera);
        int makeBetweenZeroAnd255(double value);
        void writeImageToPPMFile(Camera *camera);
        void convertPPMToPNG(std::string ppmFileName);
        void forwardRenderingPipeline(Camera *camera);

        void modelingTransformation(Camera *camera, Model *model,  Mesh *mesh);
        void cameraTransformation(Camera *camera, Model *model);
        void orthographicProjection(Camera *camera, Model *model);
        void perspectiveProjection(Camera *camera, Model *model);
        Vec3* viewportTransformation(Camera *camera, Vec3*point ); // bu x_min ve y_min 0 olacak şekilde şu an bleki değişmesi lazımdır
        Vec4* vec3to4(Vec3 *f);
        Vec3* vec4to3(Vec4 *f);
        void perspectiveDivide (Vec4 *f);
        bool visible(double den, double num, double &tL, double &tE);
        bool liangBarsky(Vec3 *v1, Vec3 *v2, Color *c1_updated, Color *c2_updated);
        double lineMaker(Vec3* v1, Vec3* v2,int x, int y);
        void lineRasterizer(Vec3* v1,Vec3* v2, Camera *cam,Color * c1_updated,Color* c2_updated);
        void triangleRasterizer(Vec3* v1, Vec3* v2, Vec3* v3, Camera *cam);
        Color substraction(Color c1, Color c2);
        Color addition(Color c1, Color c2);
        Color divisionScalar(Color c1, double scalar);

        void translate(Translation *translation, Matrix4 *matrix);
        void scale(Scaling *scaling, Matrix4 *matrix);
        void rotate(Rotation *rotation, Matrix4 *matrix);
        Vec3* copyVec3(Vec3 *f);
        Vec3 divideVec3(Vec3 f, double scalar);

};

class TriangleOfModel{ //transform edilince triangle da transform edilecek
//colorlarını da eşleştiririz vec3le sıkıntı olmaz şef
	public:
		Vec3 *vertices[3];
		int vertexIds[3];
		Color *colors[3];
		TriangleOfModel(Triangle t,Scene *scene){
			for(int i=0;i<3;i++){
				this->vertexIds[i] = t.vertexIds[i];
				this->vertices[i] = scene->vertices[t.vertexIds[i]-1];
				this->colors[i] = scene->colorsOfVertices[t.vertexIds[i]-1];
			}
		}
};



#endif

