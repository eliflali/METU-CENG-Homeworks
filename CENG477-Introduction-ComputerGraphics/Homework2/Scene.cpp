#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"
#include "Color.h"

using namespace tinyxml2;
using namespace std;
/*
	Parses XML file
*/

int counter  = 0;
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *xmlElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *rootNode = xmlDoc.FirstChild();

	// read background color
	xmlElement = rootNode->FirstChildElement("BackgroundColor");
	str = xmlElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	xmlElement = rootNode->FirstChildElement("Culling");
	if (xmlElement != NULL)
	{
		str = xmlElement->GetText();

		if (strcmp(str, "enabled") == 0)
		{
			this->cullingEnabled = true;
		}
		else
		{
			this->cullingEnabled = false;
		}
	}

	// read cameras
	xmlElement = rootNode->FirstChildElement("Cameras");
	XMLElement *camElement = xmlElement->FirstChildElement("Camera");
	XMLElement *camFieldElement;
	while (camElement != NULL)
	{
		Camera *camera = new Camera();

		camElement->QueryIntAttribute("id", &camera->cameraId);

		// read projection type
		str = camElement->Attribute("type");

		if (strcmp(str, "orthographic") == 0)
		{
			camera->projectionType = ORTOGRAPHIC_PROJECTION;
		}
		else
		{
			camera->projectionType = PERSPECTIVE_PROJECTION;
		}

		camFieldElement = camElement->FirstChildElement("Position");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

		camFieldElement = camElement->FirstChildElement("Gaze");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

		camFieldElement = camElement->FirstChildElement("Up");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

		camera->gaze = normalizeVec3(camera->gaze);
		camera->u = crossProductVec3(camera->gaze, camera->v);
		camera->u = normalizeVec3(camera->u);

		camera->w = inverseVec3(camera->gaze);
		camera->v = crossProductVec3(camera->u, camera->gaze);
		camera->v = normalizeVec3(camera->v);

		camFieldElement = camElement->FirstChildElement("ImagePlane");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &camera->left, &camera->right, &camera->bottom, &camera->top,
			   &camera->near, &camera->far, &camera->horRes, &camera->verRes);

		camFieldElement = camElement->FirstChildElement("OutputName");
		str = camFieldElement->GetText();
		camera->outputFilename = string(str);

		this->cameras.push_back(camera);

		camElement = camElement->NextSiblingElement("Camera");
	}

	// read vertices
	xmlElement = rootNode->FirstChildElement("Vertices");
	XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (vertexElement != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = vertexElement->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = vertexElement->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		this->vertices.push_back(vertex);
		this->colorsOfVertices.push_back(color);

		vertexElement = vertexElement->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	xmlElement = rootNode->FirstChildElement("Translations");
	XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
	while (translationElement != NULL)
	{
		Translation *translation = new Translation();

		translationElement->QueryIntAttribute("id", &translation->translationId);

		str = translationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		this->translations.push_back(translation);

		translationElement = translationElement->NextSiblingElement("Translation");
	}

	// read scalings
	xmlElement = rootNode->FirstChildElement("Scalings");
	XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
	while (scalingElement != NULL)
	{
		Scaling *scaling = new Scaling();

		scalingElement->QueryIntAttribute("id", &scaling->scalingId);
		str = scalingElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		this->scalings.push_back(scaling);


		scalingElement = scalingElement->NextSiblingElement("Scaling");
	}

	// read rotations
	xmlElement = rootNode->FirstChildElement("Rotations");
	XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
	while (rotationElement != NULL)
	{
		Rotation *rotation = new Rotation();

		rotationElement->QueryIntAttribute("id", &rotation->rotationId);
		str = rotationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		this->rotations.push_back(rotation);

		rotationElement = rotationElement->NextSiblingElement("Rotation");
	}

	// read meshes
	xmlElement = rootNode->FirstChildElement("Meshes");

	XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
	while (meshElement != NULL)
	{
		Mesh *mesh = new Mesh();

		meshElement->QueryIntAttribute("id", &mesh->meshId);

		// read projection type
		str = meshElement->Attribute("type");

		if (strcmp(str, "wireframe") == 0)
		{
			mesh->type = WIREFRAME_MESH;
		}
		else
		{
			mesh->type = SOLID_MESH;
		}

		// read mesh transformations
		XMLElement *meshTransformationsElement = meshElement->FirstChildElement("Transformations");
		XMLElement *meshTransformationElement = meshTransformationsElement->FirstChildElement("Transformation");

		while (meshTransformationElement != NULL)
		{
			char transformationType;
			int transformationId;

			str = meshTransformationElement->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			mesh->transformationTypes.push_back(transformationType);
			mesh->transformationIds.push_back(transformationId);

			meshTransformationElement = meshTransformationElement->NextSiblingElement("Transformation");
		}

		mesh->numberOfTransformations = mesh->transformationIds.size();

		// read mesh faces
		char *row;
		char *cloneStr;
		int v1, v2, v3;
		XMLElement *meshFacesElement = meshElement->FirstChildElement("Faces");
		str = meshFacesElement->GetText();
		cloneStr = strdup(str);

		row = strtok(cloneStr, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

			if (result != EOF)
			{
				mesh->triangles.push_back(Triangle(v1, v2, v3));
			}
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		this->meshes.push_back(mesh);

		meshElement = meshElement->NextSiblingElement("Mesh");
	}
}

void Scene::assignColorToPixel(int i, int j, Color c)
{
	this->image[i][j].r = c.r;
	this->image[i][j].g = c.g;
	this->image[i][j].b = c.b;

}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;
			vector<double> rowOfDepths;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
				rowOfDepths.push_back(1.01);
			}

			this->image.push_back(rowOfColors);
			this->depth.push_back(rowOfDepths);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				assignColorToPixel(i, j, this->backgroundColor);
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFilename.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFilename << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
*/
void Scene::convertPPMToPNG(string ppmFileName)
{
	string command;

	// TODO: Change implementation if necessary.
	command = "magick convert " + ppmFileName + " " + ppmFileName + ".png";
	system(command.c_str());
}

/*
	Transformations, clipping, culling, rasterization are done here.
	1.transformations
	1.1 modeling transformation
		1.1.1 translation
		1.1.2 scaling
		1.1.3 rotationa
	1.2 camera transformation
	1.3 orthogrsaphic/perspective projection
	1.4 viewport transformation
2.clipping algo
*** done up to here ***
3.rasterization
	2.1 midpoint
	2.2 triangle rasterization
4.depth buffer 
*/
void Scene::forwardRenderingPipeline(Camera *camera)
{
	// TODO: Implement this function
	int meshSize = meshes.size();
	for(int i = 0 ;i < meshSize;i++){
		Model *model = new Model(meshes[i],this);
		modelingTransformation(camera,model,meshes[i]);
		cameraTransformation(camera,model);
		orthographicProjection(camera,model);
		perspectiveProjection(camera,model);
		int triangleSize = meshes[i]->numberOfTriangles;
		Matrix4 camToProject;
		Matrix4 modelToCamera = multiplyMatrixWithMatrix(model->cameraModel,model->modelingTransform);
		if(camera->projectionType == ORTOGRAPHIC_PROJECTION){
				camToProject = multiplyMatrixWithMatrix(model->orthographicModel,modelToCamera);
			}
		else{
				camToProject = multiplyMatrixWithMatrix(model->perspectiveModel,modelToCamera);
			}
		for(int j = 0; j < triangleSize;j++){
			TriangleOfModel *triangle = new TriangleOfModel(meshes[i]->triangles[j],this);
			for(int k = 0 ; k < 3 ;k++){
				Vec4 *vertex = vec3to4(triangle->vertices[k]);
				*vertex = multiplyMatrixWithVec4(camToProject,*vertex);
				perspectiveDivide(vertex);
				triangle->vertices[k] = vec4to3(vertex);
				delete vertex;
			}
			if(this->cullingEnabled ){
				Vec3 edge1 = subtractVec3(*triangle->vertices[2],*triangle->vertices[1]);
				Vec3 edge2= subtractVec3(*triangle->vertices[1],*triangle->vertices[0]);
				Vec3 normal = normalizeVec3(crossProductVec3(edge1,edge2));
				Vec3 finale = divideVec3(addVec3(addVec3(*triangle->vertices[0],*triangle->vertices[1]),*triangle->vertices[2]),3);
				if(dotProductVec3(normal,finale) >= 0){
					continue; // if backface, go to next triangle
				}
			}
			//clipping
			if(meshes[i]->type == WIREFRAME_MESH){
				bool e1_clip,e2_clip,e3_clip;
				Vec3* edge1_1 = copyVec3(triangle->vertices[0]);
				Vec3* edge1_2 = copyVec3(triangle->vertices[1]);
				Vec3* edge2_1 = copyVec3(triangle->vertices[1]);
				Vec3* edge2_2 = copyVec3(triangle->vertices[2]);
				Vec3* edge3_1 = copyVec3(triangle->vertices[0]);
				Vec3* edge3_2 = copyVec3(triangle->vertices[2]);
				Vec3* finalEdges[6];
				Color *edge1_1_color = new Color();
				Color *edge1_2_color = new Color();
				Color *edge2_1_color = new Color();
				Color *edge2_2_color = new Color();
				Color *edge3_1_color = new Color();
				Color *edge3_2_color = new Color();				
				e1_clip = liangBarsky(edge1_1,edge1_2,edge1_1_color,edge1_2_color);
				e2_clip = liangBarsky(edge2_1,edge2_2,edge2_1_color,edge2_2_color);
				e3_clip = liangBarsky(edge3_1,edge3_2,edge3_1_color,edge3_2_color);
				if(e1_clip){
					finalEdges[0] = viewportTransformation(camera,edge1_1);
					finalEdges[1] = viewportTransformation(camera,edge1_2);
					lineRasterizer(finalEdges[0],finalEdges[1],camera,edge1_1_color,edge1_2_color);
				}
				if(e2_clip){	
					finalEdges[2] = viewportTransformation(camera,edge2_1);
					finalEdges[3] = viewportTransformation(camera,edge2_2);
					lineRasterizer(finalEdges[2],finalEdges[3],camera,edge2_1_color,edge2_2_color);
					}
				if(e3_clip){
					finalEdges[4] = viewportTransformation(camera,edge3_1);
					finalEdges[5] = viewportTransformation(camera,edge3_2);

					lineRasterizer(finalEdges[4],finalEdges[5],camera,edge3_1_color,edge3_2_color);
				}
			}
			else{//solid gang
				Vec3* endV1 = viewportTransformation(camera,triangle->vertices[0]);
                Vec3* endV2 = viewportTransformation(camera,triangle->vertices[1]);
                Vec3* endV3 = viewportTransformation(camera,triangle->vertices[2]);
                triangleRasterizer(endV1, endV2, endV3, camera);
				//triangle rasterization
			}

		}		
	}
    counter = 0;
}

void Scene::cameraTransformation(Camera *camera, Model *model )
{
	Matrix4 *camModel  = &(model->cameraModel);
	/* camera->gaze = normalizeVec3(camera->gaze);
	camera->u = crossProductVec3(camera->gaze, camera->v);
	camera->u = normalizeVec3(camera->u);
	camera->w = inverseVec3(camera->gaze);
	camera->v = crossProductVec3(camera->u, camera->gaze);
	camera->v = normalizeVec3(camera->v); */
	camModel->values[0][0] = camera->u.x;
	camModel->values[0][1] = camera->u.y;
	camModel->values[0][2] = camera->u.z;
	camModel->values[0][3] = -1 * (camera->u.x * camera->position.x + camera->u.y * camera->position.y + camera->u.z * camera->position.z);
	camModel->values[1][0] = camera->v.x;
	camModel->values[1][1] = camera->v.y;
	camModel->values[1][2] = camera->v.z;
	camModel->values[1][3] = -1 * (camera->v.x * camera->position.x + camera->v.y * camera->position.y + camera->v.z * camera->position.z);
	camModel->values[2][0] = camera->w.x;
	camModel->values[2][1] = camera->w.y;
	camModel->values[2][2] = camera->w.z;
	camModel->values[2][3] = -1 * (camera->w.x * camera->position.x + camera->w.y * camera->position.y + camera->w.z * camera->position.z);
	camModel->values[3][0] = 0;
	camModel->values[3][1] = 0;
	camModel->values[3][2] = 0;
	camModel->values[3][3] = 1;
}
void Scene::orthographicProjection(Camera *camera, Model *model )
{
	Matrix4 *orthographicProjection = &(model->orthographicModel);
	orthographicProjection->values[0][0] = 2 / (camera->right - camera->left);
	orthographicProjection->values[0][1] = 0;
	orthographicProjection->values[0][2] = 0;
	orthographicProjection->values[0][3] = -1 * (camera->right + camera->left) / (camera->right - camera->left);
	orthographicProjection->values[1][0] = 0;
	orthographicProjection->values[1][1] = 2 / (camera->top - camera->bottom);
	orthographicProjection->values[1][2] = 0;
	orthographicProjection->values[1][3] = -1 * (camera->top + camera->bottom) / (camera->top - camera->bottom);
	orthographicProjection->values[2][0] = 0;
	orthographicProjection->values[2][1] = 0;
	orthographicProjection->values[2][2] = -1* (2 / (camera->far - camera->near));
	orthographicProjection->values[2][3] = -1 * (camera->near + camera->far) / (camera->far - camera->near);
	orthographicProjection->values[3][0] = 0;
	orthographicProjection->values[3][1] = 0;
	orthographicProjection->values[3][2] = 0;
	orthographicProjection->values[3][3] = 1;


}
void Scene::perspectiveProjection(Camera *camera, Model *model ) //perspective to orthographic
{
	Matrix4 *perspectiveProjection = &(model->perspectiveModel);
	perspectiveProjection->values[0][0] = 2 * camera->near / (camera->right - camera->left);
	perspectiveProjection->values[0][1] = 0;
	perspectiveProjection->values[0][2] = (camera->right + camera->left) / (camera->right - camera->left);
	perspectiveProjection->values[0][3] = 0;
	perspectiveProjection->values[1][0] = 0;
	perspectiveProjection->values[1][1] = 2 * camera->near / (camera->top - camera->bottom);
	perspectiveProjection->values[1][2] = (camera->top + camera->bottom) / (camera->top - camera->bottom);
	perspectiveProjection->values[1][3] = 0;
	perspectiveProjection->values[2][0] = 0;
	perspectiveProjection->values[2][1] = 0;
	perspectiveProjection->values[2][2] = -1 * (camera->far + camera->near) / (camera->far - camera->near);
	perspectiveProjection->values[2][3] = -1 * (2 * camera->far * camera->near) / (camera->far - camera->near);
	perspectiveProjection->values[3][0] = 0;
	perspectiveProjection->values[3][1] = 0;
	perspectiveProjection->values[3][2] = -1;
	perspectiveProjection->values[3][3] = 0;
}

Vec3* Scene::viewportTransformation(Camera *camera, Vec3*point ) // bu x_min ve y_min 0 olacak şekilde şu an bleki değişmesi lazımdır
{
    /* point->x = (point->x + 1) * (camera->horRes )-1 / 2.0;
    point->y = (point->y + 1) * (camera->verRes )-1 / 2.0;
    point->z = (point->z + 1) / 2.0; */
    Vec3 * newPoint = new Vec3();
    newPoint->x = ((point->x + 1) * (camera->horRes )-1) / 2.0;
    newPoint->y = ((point->y + 1) * (camera->verRes )-1) / 2.0;
    newPoint->z = (point->z + 1) / 2.0;
    newPoint->colorId = point->colorId;
    return newPoint;

}
/*Vec3* Scene::viewportTransformation(Camera *camera, Vec3*point ) // bu x_min ve y_min 0 olacak şekilde şu an bleki değişmesi lazımdır
{
	/* point->x = (point->x + 1) * (camera->horRes )-1 / 2.0;
	point->y = (point->y + 1) * (camera->verRes )-1 / 2.0;
	point->z = (point->z + 1) / 2.0; */
	/*Vec3 * newPoint = new Vec3();
	newPoint->x = ((point->x + 1) * (camera->horRes )-1) / 2.0;
	newPoint->y = ((point->y + 1) * (camera->verRes )-1) / 2.0;
	newPoint->z = (point->z + 1) / 2.0;
	newPoint->colorId = point->colorId;
	return newPoint;

}*/
void Scene::modelingTransformation(Camera *camera, Model *model,  Mesh *mesh)
{
	int transSize = mesh->numberOfTransformations;
	for(int i = 0; i < transSize; i++) {
		char type = mesh->transformationTypes[i];
		int id = mesh->transformationIds[i] - 1;
		Matrix4 *matrix = new Matrix4();
		switch (type)		
		{
		case 't':
			translate(translations[id], matrix);
			model->modelingTransform = multiplyMatrixWithMatrix(*matrix, model->modelingTransform);
			break;
		case 's':
            std::cout<< "line 563" << std::endl;
			scale(scalings[id], matrix);
            std::cout<< "line 564" << std::endl;
			model->modelingTransform = multiplyMatrixWithMatrix(*matrix, model->modelingTransform);
			break;
		case 'r':
			rotate(rotations[id], matrix);
			model->modelingTransform = multiplyMatrixWithMatrix(*matrix, model->modelingTransform);
			break;
		default:
			break;
		}
		delete matrix;
	}
}
void Scene::translate(Translation *translation, Matrix4 *matrix)
{
	for(int i = 0; i < 4; i++) {
		matrix->values[i][i] = 1;
	}
	matrix->values[0][3] = translation->tx;
	matrix->values[1][3] = translation->ty;
	matrix->values[2][3] = translation->tz;
}
void Scene::scale(Scaling *scaling, Matrix4 *matrix)
{
	matrix->values[0][0] = scaling->sx;
	matrix->values[1][1] = scaling->sy;
	matrix->values[2][2] = scaling->sz;
    matrix->values[3][3] = 1;
}
/*void Scene::rotate(Rotation *rotation, Matrix4 *matrix)
{
	double angle = rotation->angle;
	Vec3 *rotationVector = new Vec3();
	rotationVector->x = rotation->ux;
	rotationVector->y = rotation->uy;
	rotationVector->z = rotation->uz;
	*rotationVector = normalizeVec3(*rotationVector);
	double ux = rotation->ux;
	double uy = rotation->uy;
	double uz = rotation->uz;
	double radian = angle * M_PI / 180.0;
	double cosA = cos(radian);
	double sinA = sin(radian);
	matrix->values[0][0] = cosA + ux * ux * (1 - cosA);
	matrix->values[0][1] = ux * uy * (1 - cosA) - uz * sinA;
	matrix->values[0][2] = ux * uz * (1 - cosA) + uy * sinA;
	matrix->values[0][3] = 0;
	matrix->values[1][0] = uy * ux * (1 - cosA) + uz * sinA;
	matrix->values[1][1] = cosA + uy * uy * (1 - cosA);
	matrix->values[1][2] = uy * uz * (1 - cosA) - ux * sinA;
	matrix->values[1][3] = 0;
	matrix->values[2][0] = uz * ux * (1 - cosA) - uy * sinA;
	matrix->values[2][1] = uz * uy * (1 - cosA) + ux * sinA;
	matrix->values[2][2] = cosA + uz * uz * (1 - cosA);
	matrix->values[2][3] = 0;
	matrix->values[3][0] = 0;
	matrix->values[3][1] = 0;
	matrix->values[3][2] = 0;
	matrix->values[3][3] = 1;
	delete rotationVector;
}*/

void Scene::rotate(Rotation *rotation, Matrix4 *matrix)
{
    double angle = rotation->angle;
    Vec3 rotationVector = normalizeVec3(Vec3(rotation->ux, rotation->uy, rotation->uz));
    double ux = rotationVector.x;
    double uy = rotationVector.y;
    double uz = rotationVector.z;
    double radian = angle * M_PI / 180.0;
    double cosA = cos(radian);
    double sinA = sin(radian);

    matrix->values[0][0] = cosA + ux * ux * (1 - cosA);
    matrix->values[0][1] = ux * uy * (1 - cosA) - uz * sinA;
    matrix->values[0][2] = ux * uz * (1 - cosA) + uy * sinA;
    matrix->values[0][3] = 0;
    matrix->values[1][0] = uy * ux * (1 - cosA) + uz * sinA;
    matrix->values[1][1] = cosA + uy * uy * (1 - cosA);
    matrix->values[1][2] = uy * uz * (1 - cosA) - ux * sinA;
    matrix->values[1][3] = 0;
    matrix->values[2][0] = uz * ux * (1 - cosA) - uy * sinA;
    matrix->values[2][1] = uz * uy * (1 - cosA) + ux * sinA;
    matrix->values[2][2] = cosA + uz * uz * (1 - cosA);
    matrix->values[2][3] = 0;
    matrix->values[3][0] = 0;
    matrix->values[3][1] = 0;
    matrix->values[3][2] = 0;
    matrix->values[3][3] = 1;
}

Vec4* Scene::vec3to4(Vec3 *f){
	Vec4 *v = new Vec4();
	v->x = f->x;
	v->y = f->y;
	v->z = f->z;
	v->t = 1;
	v->colorId = f->colorId;
	return v;
}
Vec3* Scene::vec4to3(Vec4 *f){

	Vec3 *v = new Vec3();
	v->x = f->x;
	v->y = f->y;
	v->z = f->z;
	v->colorId = f->colorId;
	return v;
}
Vec3* Scene::copyVec3(Vec3 *f){
	Vec3 *v = new Vec3();
	v->x = f->x;
	v->y = f->y;
	v->z = f->z;
	v->colorId = f->colorId;
	return v;
}
void Scene::perspectiveDivide (Vec4 *f){
	f->x = f->x / f->t;
	f->y = f->y / f->t;
	f->z = f->z / f->t;
	f->t  = 1;

}
bool Scene::visible(double den, double num, double &tL, double &tE){
	double t;
	if(den > 0){
		t = num/den;
		if(t > tL){
			return false;
		}
		else if(t > tE){
			tE = t;
		}
	}
	else if(den < 0){
		t = num/den;
		if(t < tE){
			return false;
		}
		else if(t < tL){
			tL = t;
		}
	}
	else if(num > 0){
		return false;
	}
	return true;
}
bool Scene::liangBarsky(Vec3 *v1, Vec3 *v2,Color *c1_updated,Color *c2_updated ){

	double tL = 1;
	double tE = 0;
	double w = 1;
	bool seen = false;
	double dx = v2->x - v1->x;
	double dy = v2->y - v1->y;
	double dz = v2->z - v1->z;
	*c1_updated = *(this->colorsOfVertices[v1->colorId-1]);
	*c2_updated = *(this->colorsOfVertices[v2->colorId-1]);
	/* if(visible(dx,-w-v1->x,tL,tE) && visible(-dx,-w+v1->x,tL,tE) &&
    visible(dy,-w-v1->y,tL,tE) && visible(-dy,-w+v1->y,tL,tE) && 
	visible(dz,-w-v1->z,tL,tE) && visible(-dz,-w+v1->z,tL,tE)){ */
	if(visible(dx,-w-v1->x,tL,tE)){
		if (visible(-dx,-w+v1->x,tL,tE)){
			if(visible(dy,-w-v1->y,tL,tE)){
				if(visible(-dy,-w+v1->y,tL,tE)){
					if(visible(dz,-w-v1->z,tL,tE)){
						if(visible(-dz,-w+v1->z,tL,tE)){
							if(tE > 0){
								v1->x = v1->x + tE * dx;
								v1->y = v1->y + tE * dy;
								v1->z = v1->z + tE * dz;
								c1_updated->r = c1_updated->r + tE * (c2_updated->r - c1_updated->r);
								c1_updated->g = c1_updated->g + tE * (c2_updated->g - c1_updated->g);
								c1_updated->b = c1_updated->b + tE * (c2_updated->b - c1_updated->b);
							}
							if(tL < 1){
								v2->x = v1->x + tL * dx;
								v2->y = v1->y + tL * dy;
								v2->z = v1->z + tL * dz;
								c2_updated->r = c1_updated->r + tL * (c2_updated->r - c1_updated->r);
								c2_updated->g = c1_updated->g + tL * (c2_updated->g - c1_updated->g);
								c2_updated->b = c1_updated->b + tL * (c2_updated->b - c1_updated->b);
							}
							seen = true;
						}
					}
				}
			}
		}
	}
	return seen;
}						
					
double Scene::lineMaker(Vec3* v1, Vec3* v2,int x, int y){
	return x*(round(v1->y)-round(v2->y))+y*(round(v2->x)-round(v1->x))+
	round(v1->x)*round(v2->y)-round(v2->x)*round(v1->y);
}
Color Scene::substraction(Color c1, Color c2){
    Color c;
    c.r = c1.r - c2.r;
    c.g = c1.g - c2.g;
    c.b = c1.b - c2.b;
    return c;
}
Color Scene::addition(Color c1, Color c2){
    Color c;
    c.r = c1.r + c2.r;
    c.g = c1.g + c2.g;
    c.b = c1.b + c2.b;
    return c;
}
Color Scene::divisionScalar(Color c1, double scalar){
    Color c;
    c.r = c1.r / scalar;
    c.g = c1.g / scalar;
    c.b = c1.b / scalar;
    return c;
}
void Scene::lineRasterizer(Vec3* v1,Vec3* v2, Camera *cam,Color* c1_updated,Color* c2_updated){
	int e1_x = round(v1->x);
	int e1_y = round(v1->y);
	int e2_x = round(v2->x);
	int e2_y = round(v2->y); 
/* 	int edges[4] = {round(v1->x),round(v1->y),round(v2->x),round(v2->y)};
 */

    Color e1_c = *c1_updated;
	Color e2_c = *c2_updated;
	int coords[4] ;
    /*x0, y0, x1, y1*/
	Color finalc1,finalc2;
	if(abs(e2_y-e1_y) < abs(e2_x - e1_x)){
		if(e1_x > e2_x){
			coords[0] = e2_x;
			coords[1] = e2_y;
			coords[2] = e1_x;
			coords[3] = e1_y;
			finalc1 = e2_c;
			finalc2 = e1_c;
		}
		else{
			coords[0] = e1_x;
			coords[1] = e1_y;
			coords[2] = e2_x;
			coords[3] = e2_y;
			finalc1 = e1_c;
			finalc2 = e2_c;
		}
		int directionx = coords[2] - coords[0];
		int directiony = coords[3] - coords[1];
		int yinitial  = 1;
		if(directiony < 0){
			yinitial = -1;
			directiony = -directiony;
		}
		Color c = finalc1;
/* 		Color directionc =  Color::divisionScalar(Color::substraction(finalc2,finalc1),directionx));
 */
		Color directionc = divisionScalar(substraction(finalc2,finalc1),directionx);
		int finalDirection = 2*directiony - directionx;
		int finaly = coords[1];
		double z_value;
		for(int x = coords[0]; x < coords[2];x++){
			z_value =  (x - coords[0]) * (v2->z - v1->z) / (coords[2] - coords[0]) + v1->z;
			if(x < cam->horRes && finaly < cam->verRes && x >= 0 && finaly >= 0 && this->depth[x][finaly] > z_value ){
				this->image[x][finaly] = c;
				this->depth[x][finaly] = z_value;
			}
            counter++;
			if(finalDirection > 0){
				finaly += yinitial;
				finalDirection += 2*(directiony - directionx);
			}
			else{
				finalDirection += 2*directiony;
			}
			c = addition(c,directionc);
		}

	}
	else{
		if(e1_y > e2_y){
			coords[0] = e2_x;
			coords[1] = e2_y;
			coords[2] = e1_x;
			coords[3] = e1_y;
			finalc1 = e2_c;
			finalc2 = e1_c;
		}
		else{
			coords[0] = e1_x;
			coords[1] = e1_y;
			coords[2] = e2_x;
			coords[3] = e2_y;
			finalc1 = e1_c;
			finalc2 = e2_c;
		}
		int directionx = coords[2] - coords[0];
		int directiony = coords[3] - coords[1];
		int xinitial  = 1;
		if(directionx < 0){
			xinitial = -1;
			directionx = -directionx;
		}
		Color c = finalc1;
        counter++;
		Color directionc = divisionScalar(substraction(finalc2,finalc1),directiony);
		int finalDirection = 2*directionx - directiony;
		int finalx = coords[0];
		double z_value;
		for(int y = coords[1]; y < coords[3];y++){
			z_value =  (y - coords[1]) * (v2->z - v1->z) / (coords[3] - coords[1]) + v1->z;
			if(finalx < cam->horRes && y < cam->verRes && finalx >= 0 && y >= 0 && this->depth[finalx][y] > z_value ){
				this->image[finalx][y] = c;
				this->depth[finalx][y] = z_value;
			}
			if(finalDirection > 0){
				finalx += xinitial;
				finalDirection += 2*(directionx - directiony);
			}
			else{
				finalDirection += 2*directionx;
			}
			c = addition(c,directionc);
		}
	}



}

void Scene::triangleRasterizer(Vec3* v1, Vec3* v2, Vec3* v3, Camera *cam)
{
    Color colorV1 = *(this->colorsOfVertices[v1->colorId-1]);
    Color colorV2 = *(this->colorsOfVertices[v2->colorId-1]);
    Color colorV3 = *(this->colorsOfVertices[v3->colorId-1]);
    int xMin = round(min(min(v1->x,v2->x),v3->x));
    int yMin = round(min(min(v1->y,v2->y),v3->y));
    int xMax = round(max(max(v1->x,v2->x),v3->x));
    int yMax = round(max(max(v1->y,v2->y),v3->y));
    double colorDivider1 = lineMaker(v2,v3,round(v1->x),round(v1->y));
    double colorDivider2 = lineMaker(v3,v1,round(v2->x),round(v2->y));
    double colorDivider3 = lineMaker( v1,v2,round(v3->x),round(v3->y));
    for(int y = yMin; y < yMax;y++){
        for(int x = xMin; x < xMax;x++){
            double alpha = lineMaker(v2,v3,x,y) / colorDivider1;
            double beta = lineMaker(v3,v1,x,y) / colorDivider2;
            double gamma = lineMaker(v1,v2,x,y) / colorDivider3;
            double z_value = alpha * v1->z + beta * v2->z + gamma * v3->z;
            if(alpha >= 0 && beta >= 0 && gamma >= 0){
                Vec3 c1 = Vec3(colorV1.r, colorV1.g, colorV1.b);
                Vec3 c2 = Vec3(colorV2.r, colorV2.g, colorV2.b);
                Vec3 c3 = Vec3(colorV3.r, colorV3.g, colorV3.b);
                Vec3 result = addVec3(addVec3(multiplyVec3WithScalar(c1,alpha),multiplyVec3WithScalar(c2,beta)),multiplyVec3WithScalar(c3,gamma));
                Color c = Color(result.getNthComponent(0), result.getNthComponent(1),result.getNthComponent(2));
                if(x < cam->horRes && y < cam->verRes && x >= 0 && y >= 0 && this->depth[x][y] > z_value){
                    this->image[x][y] = c;
                    this->depth[x][y] = z_value;
                }
            }
        }
    }
}
Vec3 Scene::divideVec3(Vec3 f, double scalar){
	Vec3 v;
	v.x = f.x / scalar;
	v.y = f.y / scalar;
	v.z = f.z / scalar;
	return v;
}
