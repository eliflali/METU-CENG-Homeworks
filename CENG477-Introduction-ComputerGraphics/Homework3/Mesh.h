#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <string>
#include <vector>
#include "Shader.h"
#define BUFFER_OFFSET(i) ((char*)NULL + (i))
using namespace std;

struct Vertex
{
    Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
    GLfloat x, y, z;
};

struct Texture
{
    Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
    GLfloat u, v;
};

struct Normal
{
    Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
    GLfloat x, y, z;
};

struct Face
{
    Face(int v[], int t[], int n[]) {
        vIndex[0] = v[0];
        vIndex[1] = v[1];
        vIndex[2] = v[2];
        tIndex[0] = t[0];
        tIndex[1] = t[1];
        tIndex[2] = t[2];
        nIndex[0] = n[0];
        nIndex[1] = n[1];
        nIndex[2] = n[2];
    }
    GLuint vIndex[3], tIndex[3], nIndex[3];
};

class Mesh {
public:
    vector<Vertex>       vertices;
    vector<Normal>       normals;
    vector<Face>         faces;
    vector<Texture>      textures;
    unsigned int VAO;

    Mesh(const string& fileName) {
        ParseObj(fileName);
        setupMesh();    
    }

    // render the mesh
    void Draw(Shader &shader) 
    {
        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        size_t totalDataSize = vertices.size() * sizeof(Vertex) + normals.size() * sizeof(Normal);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Load vertex data into VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, totalDataSize, 0, GL_STATIC_DRAW);

        // Upload vertex and normal data to the GPU
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);
        glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), normals.size() * sizeof(Normal), &normals[0]);

        // Set the vertex attribute pointers for vertex positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Set the vertex attribute pointers for vertex normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Normal), BUFFER_OFFSET(vertices.size() * sizeof(Vertex)));
        glEnableVertexAttribArray(1);

        // Load index data into EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(Face), &faces[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    bool ParseObj(const string& fileName) {
        fstream myfile;

        // Open the input file
        myfile.open(fileName.c_str(), std::ios::in);

        // Check if the file is successfully opened
        if (myfile.is_open()) {
            string curLine;

            // Read the file line by line
            while (getline(myfile, curLine)) {
                stringstream str(curLine);
                GLfloat c1, c2, c3;
                GLuint index[9];
                string tmp;

                // Check if the line is non-empty
                if (curLine.length() >= 2) {
                    // Process vertex data
                    if (curLine[0] == 'v') {
                        if (curLine[1] == 't') { // Texture coordinate
                            str >> tmp; // consume "vt"
                            str >> c1 >> c2;
                            textures.push_back(Texture(c1, c2));
                        }
                        else if (curLine[1] == 'n') { // Normal vector
                            str >> tmp; // consume "vn"
                            str >> c1 >> c2 >> c3;
                            normals.push_back(Normal(c1, c2, c3));
                        }
                        else { // Vertex position
                            str >> tmp; // consume "v"
                            str >> c1 >> c2 >> c3;
                            vertices.push_back(Vertex(c1, c2, c3));
                        }
                    }
                        // Process face data
                    else if (curLine[0] == 'f') {
                        str >> tmp; // consume "f"
                        char c;
                        int vIndex[3], nIndex[3], tIndex[3];
                        // Parse indices of vertex/texture/normal for each vertex of the face
                        str >> vIndex[0]; str >> c >> c; // consume "//"
                        str >> nIndex[0];
                        str >> vIndex[1]; str >> c >> c; // consume "//"
                        str >> nIndex[1];
                        str >> vIndex[2]; str >> c >> c; // consume "//"
                        str >> nIndex[2];

                        // Assert to check that vertex and normal indices are matching
                        assert(vIndex[0] == nIndex[0] &&
                            vIndex[1] == nIndex[1] &&
                            vIndex[2] == nIndex[2]); // a limitation for now

                        // Adjust indices to be 0-based instead of 1-based
                        for (int c = 0; c < 3; ++c) {
                            vIndex[c] -= 1;
                            nIndex[c] -= 1;
                            tIndex[c] -= 1;
                        }

                        // Add the face data
                        faces.push_back(Face(vIndex, tIndex, nIndex));
                    }
                    else {
                        // Ignore lines that are not vertex, texture, normal, or face definitions
                        cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                    }
                }
            }

            myfile.close();
        } else {
            return false; // Return false if file couldn't be opened
        }

        /*
        // The commented section would calculate normals for each vertex
        // if it were not commented out. This is typically used when
        // the OBJ file doesn't contain normal data.
        */
        assert(vertices.size() == normals.size());   
        return true; // Return true if parsing was successful
    }
};
#endif