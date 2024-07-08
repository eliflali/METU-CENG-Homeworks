#include <iostream>
#include <vector>
#include "Scene.h"

using namespace std;

Scene *scene;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Please run the rasterizer as:" << endl
             << "\t./rasterizer <input_file_name>" << endl;
        return 1;
    }
    else
    {
        const char *xmlPath = argv[1];

        scene = new Scene(xmlPath);
        std::cout<<"main 22"<< std::endl;
        for (int i = 0; i < scene->cameras.size(); i++)
        {
            // initialize image with basic values
            scene->initializeImage(scene->cameras[i]);
            std::cout<<"main 27"<< std::endl;

            // do forward rendering pipeline operations
            scene->forwardRenderingPipeline(scene->cameras[i]);
            std::cout<<"main 31"<< std::endl;

            // generate PPM file
            scene->writeImageToPPMFile(scene->cameras[i]);
            std::cout<<"main 35"<< std::endl;

            // Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
            // Change/remove implementation if necessary.
            scene->convertPPMToPNG(scene->cameras[i]->outputFilename);
            std::cout<<"main 40"<< std::endl;
        }

        return 0;
    }
}