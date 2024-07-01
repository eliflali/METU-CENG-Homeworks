#include <iostream>
#include "parser.h"
#include "ppm.h"
#include "rayGenerator.h"
#include <algorithm>
#include <vector>
#include "computeColor.h"
#include <math.h>
#include <unistd.h>
#include <pthread.h>

using namespace rayGenerator;
using namespace parser;
using namespace computeColor;

int max_recursion_depth;
std::vector<Camera> cameras;

float clamp(float value, float epsilon)
{
    float bounded = value > 255 ? 255 : round(value + epsilon);
    return bounded;
}

struct Thread {
    int startY;
    int endY;
    int width;
    int height;
    unsigned char* image;
    const Camera* camera;
    const parser::Scene* scene;
    int max_recursion_depth;
};

void* executeThread(void* arg) {
    Thread* data = static_cast<Thread*>(arg);
    int pixel = data->startY * data->width * 3;

    for(int ny = data->startY; ny < data->endY; ny++) {
        for(int nx = 0; nx < data->width; nx++) {
            Ray ray = rayGenerator::generateRay(*(data->camera), ny, nx);
            ray.depth = 0;
            Vec3f color = computePixelColor(*(data->scene), *(data->camera), ray, data->max_recursion_depth);
            data->image[pixel] = clamp(color.x, data->scene->shadow_ray_epsilon);
            data->image[pixel+1] = clamp(color.y, data->scene->shadow_ray_epsilon);
            data->image[pixel+2] = clamp(color.z, data->scene->shadow_ray_epsilon);
            pixel += 3;
        }
    }

    return nullptr;
}

int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;
    scene.loadFromXml(argv[1]);
    max_recursion_depth = scene.max_recursion_depth;
    cameras = scene.cameras;

    const long coreNumber = sysconf(_SC_NPROCESSORS_ONLN);
    std::vector<pthread_t> threads(coreNumber);
    std::vector<Thread> threadData(coreNumber);

    //for every camera angle, we must have a distinct ppm.
    for(int i = 0; i<cameras.size(); i++)
    {
        Camera camera = cameras[i];
        int width = camera.image_width;
        int height = camera.image_height;
        unsigned char* image = new unsigned char [width * height * 3];
        int threadHeight = height / coreNumber;

        for (int j = 0; j < coreNumber; ++j) {
            threadData[j].startY = j * threadHeight;
            threadData[j].endY = (j == coreNumber - 1) ? height : (j + 1) * threadHeight;
            threadData[j].width = width;
            threadData[j].height = height;
            threadData[j].image = image;
            threadData[j].camera = &camera;
            threadData[j].scene = &scene;
            threadData[j].max_recursion_depth = max_recursion_depth;
            pthread_create(&threads[j], nullptr, executeThread, &threadData[j]);
        }
        //finished:
        for (auto& thread : threads) {
            pthread_join(thread, nullptr);
        }

        write_ppm(camera.image_name.c_str(), image, width, height);
        delete[] image;
    }
}