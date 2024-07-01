#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <GL/glew.h>
//#include <OpenGL/gl3.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

GLuint gProgram[2];
GLuint groundProgram;
GLuint cubeProgram;
GLuint textProgram;
GLuint skyProgram;

GLint modelingMatrixLoc[2];
GLint viewingMatrixLoc[2];
GLint projectionMatrixLoc[2];
GLint eyePosLoc[2];

GLint groundModelingMatrixLoc;
GLint groundViewingMatrixLoc;
GLint groundProjectionMatrixLoc;
GLint groundEyePosLoc;

GLint skyModelingMatrixLoc;
GLint skyViewingMatrixLoc;
GLint skyProjectionMatrixLoc;
GLint skyEyePosLoc;
GLint skyTextureLocation;

GLint scaleLocation;
GLint offsetLocation;
GLint color1Location;
GLint color2Location;

glm::vec3 red(1.f,0.f,0.1f);
glm::vec3 yellow(1.f,1.f,0.1f);

int colorRandomizer[3];
int width, height;
double score  = 0;

GLint cubeModelingMatrixLoc;
GLint cubeViewingMatrixLoc;
GLint cubeProjectionMatrixLoc;
GLint cubeEyePosLoc;
GLint cubeScaleLocation;
GLint cubeOffsetLocation;
GLint cubeColor1Location;
GLint cubeLightColorLocation;
GLint cubeLightPosLocation;



glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::mat4 modelingMatrix;
glm::vec3 eyePos(0, 2, 0);
glm::vec3 offset(0.0,-2.0,-2.0);
glm::vec3 Coffset(0.0,-2.0,-2.0);

glm::mat4 groundProjectionMatrix;
glm::mat4 groundViewingMatrix;
glm::mat4 groundModelingMatrix;


glm::mat4 skyProjectionMatrix;
glm::mat4 skyViewingMatrix;
glm::mat4 skyModelingMatrix;

glm::mat4 cubeProjectionMatrix;
glm::mat4 cubeViewingMatrix;
glm::mat4 cubeModelingMatrix;


GLuint vao;
GLuint vaoG;
GLuint vaoSky;
GLuint vaoCube;
GLuint gTextVBO;

int activeProgramIndex = 0;

double moveSpeed = 0.05;
double gravity = -0.0025;

bool pause = false;
bool moveLeft = false, moveRight = false;
bool happy = false, faint = false;
bool finished = false;

bool restartState = false;

int hittenCube = -1;
bool hitten  = false;

float gameSpeed = 0.20;
float gameAcceleration = 0.000003;

GLuint skyTexture;

void cubeRand()
{
    int randomYellow = rand() %3;
    for(int i = 0; i < 3; i++)
    {
        if(i == randomYellow)
        {
            colorRandomizer[i] = 1;
        }
        else
        {
            colorRandomizer[i] = 0;
        }
    }
}

struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

map<GLchar, Character> Characters;

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

struct Face {
    Face(int v[], int t[], int n[]) {
        vIndex[0] = v[0];
        vIndex[1] = v[1];
        vIndex[2] = v[2];

        if (t != nullptr) {  // Check if texture indices are provided
            tIndex[0] = t[0];
            tIndex[1] = t[1];
            tIndex[2] = t[2];
        } else {
            tIndex[0] = tIndex[1] = tIndex[2] = 0;  // Safe default value
        }

        nIndex[0] = n[0];
        nIndex[1] = n[1];
        nIndex[2] = n[2];
    }
    GLuint vIndex[3], tIndex[3], nIndex[3];
};


struct Bunny
{

    double positionX = 0;
    double positionY = 0;
    double positionZ = 0;

    double velocityX = moveSpeed;
    double velocityY = 0;
    double velocityZ = 0;

    float angleX = 0;
    float angleY = 0;
    float angleZ = 0;

    const double jumpVelocity = 0.05;

    vector<Vertex> gVertices;
    vector<Texture> gTextures;
    vector<Normal> gNormals;
    vector<Face> gFaces;

    GLuint gVertexAttribBuffer, gIndexBuffer;
    GLint gInVertexLoc, gInNormalLoc;
    int gVertexDataSizeInBytes, gNormalDataSizeInBytes;

};
Bunny bunny;

// Assuming you have the same structures (Vertex, Texture, Normal, Face) as in your Bunny class
struct Quad {
    vector<Vertex> gVertices;
    vector<Texture> gTextures;
    vector<Normal> gNormals;
    vector<Face> gFaces;

    GLuint gVertexAttribBuffer, gIndexBuffer;
    GLint gInVertexLoc, gInNormalLoc;
    int gVertexDataSizeInBytes, gNormalDataSizeInBytes;
};
Quad  quad;
Quad sky;
struct Cube{
    vector<Vertex> gVertices;
    vector<Texture> gTextures;
    vector<Normal> gNormals;
    vector<Face> gFaces;

    glm::vec3 color;

    double positionX = 0;
    double positionY = 0;
    double positionZ = 0;

    GLuint gVertexAttribBuffer, gIndexBuffer;
    GLint gInVertexLoc, gInNormalLoc;
    int gVertexDataSizeInBytes, gNormalDataSizeInBytes;
};
Cube cube;
// Function to parse a Wavefront .obj file
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
                        bunny.gTextures.push_back(Texture(c1, c2));
                    }
                    else if (curLine[1] == 'n') { // Normal vector
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        bunny.gNormals.push_back(Normal(c1, c2, c3));
                    }
                    else { // Vertex position
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        bunny.gVertices.push_back(Vertex(c1, c2, c3));
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
                    bunny.gFaces.push_back(Face(vIndex, tIndex, nIndex));
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

    // Check that the number of vertices is equal to the number of normals
    assert(bunny.gVertices.size() == bunny.gNormals.size());

    return true; // Return true if parsing was successful
}

void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state	
    glUseProgram(textProgram);
    glUniform3f(glGetUniformLocation(textProgram, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },            
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }           
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)

        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}
bool ParseCube(const string& fileName){
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
                        cube.gTextures.push_back(Texture(c1, c2));
                    }
                    else if (curLine[1] == 'n') { // Normal vector
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        cube.gNormals.push_back(Normal(c1, c2, c3));
                    }
                    else { // Vertex position
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        cube.gVertices.push_back(Vertex(c1, c2, c3));                                                sky.gVertices.push_back(Vertex(c1, c2, c3));

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
                    cube.gFaces.push_back(Face(vIndex, tIndex, nIndex));
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

    // Check that the number of vertices is equal to the number of normals
    assert(cube.gVertices.size() == cube.gNormals.size());

    return true;
}
bool ParseQuad (const string& fileName) {
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
                        quad.gTextures.push_back(Texture(c1, c2));
                        sky.gTextures.push_back(Texture(c1, c2));
                    } else if (curLine[1] == 'n') { // Normal vector
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        quad.gNormals.push_back(Normal(c1, c2, c3));
                        sky.gNormals.push_back(Normal(c1, c2, c3));
                    } else { // Vertex position
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        quad.gVertices.push_back(Vertex(c1, c2, c3));
                        sky.gVertices.push_back(Vertex(c1, c2, c3));
                    }
                }
                    // Process face data
                else if (curLine[0] == 'f') {
                    str >> tmp; // consume "f"
                    char c;
                    int vIndex[3], nIndex[3], tIndex[3];
                    // Parse indices of vertex/texture/normal for each vertex of the face
                    str >> vIndex[0];
                    str >> c >> c; // consume "//"
                    str >> nIndex[0];
                    str >> vIndex[1];
                    str >> c >> c; // consume "//"
                    str >> nIndex[1];
                    str >> vIndex[2];
                    str >> c >> c; // consume "//"
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
                    quad.gFaces.push_back(Face(vIndex, tIndex, nIndex));
                    sky.gFaces.push_back(Face(vIndex, tIndex, nIndex));
                } else {
                    // Ignore lines that are not vertex, texture, normal, or face definitions
                    cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                }
            }
        }

        myfile.close();
    } else {
        return false; // Return false if file couldn't be opened
    }
    assert(quad.gVertices.size() == quad.gNormals.size());
    assert(sky.gVertices.size() == sky.gNormals.size());
    return true;
}
bool ReadDataFromFile(const string& fileName, string& data)     ///< [out] The contents of the file
{
    fstream myfile;
    // Open the input
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }
        myfile.close();
    }
    else
    {
        return false;
    }
    return true;
}
void loadSkyTexture(const std::string& img_name) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(img_name.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture" << std::endl;
        return;
    }

    glGenTextures(1, &skyTexture);
    glBindTexture(GL_TEXTURE_2D, skyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}
// Function to create and compile a vertex shader
GLuint createVS(const char* shaderName)
{
	string shaderSource;

	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource))
	{
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &shader, &length);
	glCompileShader(vs);

	char output[1024] = { 0 };
	glGetShaderInfoLog(vs, 1024, &length, output);
	printf("VS compile log: %s\n", output);

	return vs;
}
GLuint createFS(const char* shaderName)
{
	string shaderSource;

	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource))
	{
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &shader, &length);
	glCompileShader(fs);

	char output[1024] = { 0 };
	glGetShaderInfoLog(fs, 1024, &length, output);
	printf("FS compile log: %s\n", output);

	return fs;
}

void initShaders()
{
	// Create the programs
// Create the programs

	gProgram[0] = glCreateProgram();
	gProgram[1] = glCreateProgram();

	// Create the shaders for both programs

	GLuint vs1 = createVS("vert.glsl");
	GLuint fs1 = createFS("frag.glsl");

	GLuint vs2 = createVS("vert2.glsl");
	GLuint fs2 = createFS("frag2.glsl");

	// Attach the shaders to the programs

	glAttachShader(gProgram[0], vs1);
	glAttachShader(gProgram[0], fs1);

	glAttachShader(gProgram[1], vs2);
	glAttachShader(gProgram[1], fs2);

	// Link the programs

	glLinkProgram(gProgram[0]);
	GLint status;
	glGetProgramiv(gProgram[0], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	glLinkProgram(gProgram[1]);
	glGetProgramiv(gProgram[1], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	// Get the locations of the uniform variables from both programs

	for (int i = 0; i < 2; ++i)
	{
		modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
		viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
		projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
		eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
	}
    //ground time
    groundProgram = glCreateProgram();
    GLuint groundVS = createVS("groundVert.glsl");
    GLuint groundFS = createFS("groundFragment.glsl");
    glAttachShader(groundProgram, groundVS);
    glAttachShader(groundProgram, groundFS);
    glLinkProgram(groundProgram);
    glGetProgramiv(groundProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        cout << "Program link failed" << endl;
        exit(-1);
    }
    groundModelingMatrixLoc = glGetUniformLocation(groundProgram, "modelingMatrix");
    groundViewingMatrixLoc = glGetUniformLocation(groundProgram, "viewingMatrix");
    groundProjectionMatrixLoc = glGetUniformLocation(groundProgram, "projectionMatrix");
    groundEyePosLoc = glGetUniformLocation(groundProgram, "eyePos");
    scaleLocation = glGetUniformLocation(groundProgram, "scale");
    offsetLocation = glGetUniformLocation(groundProgram, "offset");
    color1Location = glGetUniformLocation(groundProgram, "color1");
    color2Location = glGetUniformLocation(groundProgram, "color2");
    //
    skyProgram = glCreateProgram();
    GLuint skyVS = createVS("skyVert.glsl");
    GLuint skyFS = createFS("skyFrag.glsl");
    assert(glGetError() == GL_NONE);
    glAttachShader(skyProgram, skyVS);
    glAttachShader(skyProgram, skyFS);
    assert(glGetError() == GL_NONE);
    glLinkProgram(skyProgram);
    glGetProgramiv(skyProgram, GL_LINK_STATUS, &status);

    if (status != GL_TRUE)
    {
        cout << "Sky program link failed" << endl;
        exit(-1);
    }
    skyModelingMatrixLoc = glGetUniformLocation(skyProgram, "modelingMatrix");
    skyViewingMatrixLoc = glGetUniformLocation(skyProgram, "viewingMatrix");
    skyProjectionMatrixLoc = glGetUniformLocation(skyProgram, "projectionMatrix");
    skyEyePosLoc = glGetUniformLocation(skyProgram, "eyePos");
    skyTextureLocation = glGetUniformLocation(skyProgram, "texture1");

    

    //now for cube
    cubeProgram = glCreateProgram();
    GLuint cubeVS = createVS("cubeVert.glsl");
    GLuint cubeFS = createFS("cubeFrag.glsl");
    glAttachShader(cubeProgram, cubeVS);
    glAttachShader(cubeProgram, cubeFS);
    glLinkProgram(cubeProgram);
    glGetProgramiv(cubeProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        cout << "Program link failed" << endl;
        exit(-1);
    }
    cubeModelingMatrixLoc = glGetUniformLocation(cubeProgram, "modelingMatrix");
    cubeViewingMatrixLoc = glGetUniformLocation(cubeProgram, "viewingMatrix");
    cubeProjectionMatrixLoc = glGetUniformLocation(cubeProgram, "projectionMatrix");
    cubeEyePosLoc = glGetUniformLocation(cubeProgram, "eyePos");
    cubeColor1Location = glGetUniformLocation(cubeProgram, "objectColor");
    cubeLightPosLocation = glGetUniformLocation(cubeProgram, "lightPos");
    cubeScaleLocation = glGetUniformLocation(cubeProgram, "scale");
    cubeOffsetLocation = glGetUniformLocation(cubeProgram, "offset");
    cubeLightColorLocation = glGetUniformLocation(cubeProgram, "lightColor");
    //now for text
    textProgram = glCreateProgram();
    GLuint textVS = createVS("vert_text.glsl");
    GLuint textFS = createFS("frag_text.glsl");
    glBindAttribLocation(textProgram, 2, "vertex");
    glAttachShader(textProgram, textVS);
    glAttachShader(textProgram, textFS);
    glLinkProgram(textProgram);
    glGetProgramiv(textProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        cout << "Program link failed" << endl;
        exit(-1);
    }



}

void initVBO()
{
	glGenVertexArrays(1, &vao);
	assert(vao > 0);
	glBindVertexArray(vao);
	cout << "vao = " << vao << endl;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &bunny.gVertexAttribBuffer);
	glGenBuffers(1, &bunny.gIndexBuffer);

	assert(bunny.gVertexAttribBuffer > 0 && bunny.gIndexBuffer > 0);

	glBindBuffer(GL_ARRAY_BUFFER, bunny.gVertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  bunny.gIndexBuffer);

	 bunny.gVertexDataSizeInBytes =  bunny.gVertices.size() * 3 * sizeof(GLfloat);
	 bunny.gNormalDataSizeInBytes =  bunny.gNormals.size() * 3 * sizeof(GLfloat);
	int indexDataSizeInBytes =  bunny.gFaces.size() * 3 * sizeof(GLuint);
	GLfloat* vertexData = new GLfloat[ bunny.gVertices.size() * 3];
	GLfloat* normalData = new GLfloat[ bunny.gNormals.size() * 3];
	GLuint* indexData = new GLuint[ bunny.gFaces.size() * 3];

	float minX = 1e6, maxX = -1e6;
	float minY = 1e6, maxY = -1e6;
	float minZ = 1e6, maxZ = -1e6;

	for (int i = 0; i <  bunny.gVertices.size(); ++i)
	{
		vertexData[3 * i] =  bunny.gVertices[i].x;
		vertexData[3 * i + 1] =  bunny.gVertices[i].y;
		vertexData[3 * i + 2] =  bunny.gVertices[i].z;

	}

	std::cout << "minX = " << minX << std::endl;
	std::cout << "maxX = " << maxX << std::endl;
	std::cout << "minY = " << minY << std::endl;
	std::cout << "maxY = " << maxY << std::endl;
	std::cout << "minZ = " << minZ << std::endl;
	std::cout << "maxZ = " << maxZ << std::endl;

	for (int i = 0; i <  bunny.gNormals.size(); ++i)
	{
		normalData[3 * i] =  bunny.gNormals[i].x;
		normalData[3 * i + 1] =  bunny.gNormals[i].y;
		normalData[3 * i + 2] =  bunny.gNormals[i].z;
	}

	for (int i = 0; i <  bunny.gFaces.size(); ++i)
	{
		indexData[3 * i] =  bunny.gFaces[i].vIndex[0];
		indexData[3 * i + 1] =  bunny.gFaces[i].vIndex[1];
		indexData[3 * i + 2] =  bunny.gFaces[i].vIndex[2];
	}


	glBufferData(GL_ARRAY_BUFFER,  bunny.gVertexDataSizeInBytes +  bunny.gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,  bunny.gVertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER,  bunny.gVertexDataSizeInBytes,  bunny.gNormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	// done copying to GPU memory; can free now from CPU memory
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( bunny.gVertexDataSizeInBytes));

    //NOW SAME FOR QUAD
    glGenVertexArrays(1, &vaoG);
    assert(vaoG > 0);
    glBindVertexArray(vaoG);
    cout << "vaoG = " << vaoG << endl;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert(glGetError() == GL_NONE);
    glGenBuffers(1, &quad.gVertexAttribBuffer);
    glGenBuffers(1, &quad.gIndexBuffer);
    assert(quad.gVertexAttribBuffer > 0 && quad.gIndexBuffer > 0);
    glBindBuffer(GL_ARRAY_BUFFER, quad.gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  quad.gIndexBuffer);
    quad.gVertexDataSizeInBytes =  quad.gVertices.size() * 3 * sizeof(GLfloat);
    quad.gNormalDataSizeInBytes =  quad.gNormals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytesG =  quad.gFaces.size() * 3 * sizeof(GLuint);
    GLfloat* vertexDataG = new GLfloat[ quad.gVertices.size() * 3];
    GLfloat* normalDataG = new GLfloat[ quad.gNormals.size() * 3];
    GLuint* indexDataG = new GLuint[ quad.gFaces.size() * 3];
    for(int i = 0; i < quad.gVertices.size(); ++i) {
        vertexDataG[3 * i] = quad.gVertices[i].x;
        vertexDataG[3 * i + 1] = quad.gVertices[i].y;
        vertexDataG[3 * i + 2] = quad.gVertices[i].z;
    }
    for(int i = 0; i < quad.gNormals.size(); ++i) {
        normalDataG[3 * i] = quad.gNormals[i].x;
        normalDataG[3 * i + 1] = quad.gNormals[i].y;
        normalDataG[3 * i + 2] = quad.gNormals[i].z;
    }
    for(int i = 0; i < quad.gFaces.size(); ++i) {
        indexDataG[3 * i] = quad.gFaces[i].vIndex[0];
        indexDataG[3 * i + 1] = quad.gFaces[i].vIndex[1];
        indexDataG[3 * i + 2] = quad.gFaces[i].vIndex[2];
    }
    glBufferData(GL_ARRAY_BUFFER, quad.gVertexDataSizeInBytes + quad.gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, quad.gVertexDataSizeInBytes, vertexDataG);
    glBufferSubData(GL_ARRAY_BUFFER, quad.gVertexDataSizeInBytes, quad.gNormalDataSizeInBytes, normalDataG);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexDataSizeInBytesG, indexDataG, GL_STATIC_DRAW);
    // done copying to GPU memory; can free now from CPU memory
    delete[] vertexDataG;
    delete[] normalDataG;
    delete[] indexDataG;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(quad.gVertexDataSizeInBytes));
    //NOW SAME FOR SKY
    glGenVertexArrays(1, &vaoSky);
    assert(vaoSky > 0);
    glBindVertexArray(vaoSky);
    cout << "vaoSky = " << vaoSky << endl;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert(glGetError() == GL_NONE);
    GLuint skyTextureBuffer;
    glGenBuffers(1, &sky.gVertexAttribBuffer);
    glGenBuffers(1, &sky.gIndexBuffer);
    glGenBuffers(1, &skyTextureBuffer);

    assert(sky.gVertexAttribBuffer > 0 && sky.gIndexBuffer > 0);
    glBindBuffer(GL_ARRAY_BUFFER, skyTextureBuffer);
    glBufferData(GL_ARRAY_BUFFER, sky.gTextures.size() * sizeof(Texture), sky.gTextures.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Texture), (void*)0);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, sky.gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  sky.gIndexBuffer);
    sky.gVertexDataSizeInBytes =  sky.gVertices.size() * 3 * sizeof(GLfloat);
    sky.gNormalDataSizeInBytes =  sky.gNormals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytesS =  sky.gFaces.size() * 3 * sizeof(GLuint);
    GLfloat* vertexDataS = new GLfloat[ sky.gVertices.size() * 3];
    GLfloat* normalDataS = new GLfloat[ sky.gNormals.size() * 3];
    GLuint* indexDataS = new GLuint[ sky.gFaces.size() * 3];
    for(int i = 0; i < sky.gVertices.size(); ++i) {
        vertexDataS[3 * i] = sky.gVertices[i].x;
        vertexDataS[3 * i + 1] = sky.gVertices[i].y;
        vertexDataS[3 * i + 2] = sky.gVertices[i].z;
    }
    for(int i = 0; i < sky.gNormals.size(); ++i) {
        normalDataS[3 * i] = sky.gNormals[i].x;
        normalDataS[3 * i + 1] = sky.gNormals[i].y;
        normalDataS[3 * i + 2] = sky.gNormals[i].z;
    }
    for(int i = 0; i < sky.gFaces.size(); ++i) {
        indexDataS[3 * i] = sky.gFaces[i].vIndex[0];
        indexDataS[3 * i + 1] = sky.gFaces[i].vIndex[1];
        indexDataS[3 * i + 2] = sky.gFaces[i].vIndex[2];
    }
    glBufferData(GL_ARRAY_BUFFER, sky.gVertexDataSizeInBytes + sky.gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sky.gVertexDataSizeInBytes, vertexDataS);
    glBufferSubData(GL_ARRAY_BUFFER, sky.gVertexDataSizeInBytes, sky.gNormalDataSizeInBytes, normalDataS);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexDataSizeInBytesS, indexDataS, GL_STATIC_DRAW);
    // done copying to GPU memory; can free now from CPU memory
    delete[] vertexDataS;
    delete[] normalDataS;
    delete[] indexDataS;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sky.gVertexDataSizeInBytes));

    
    //NOW SAME FOR CUBE
    glGenVertexArrays(1, &vaoCube);
    assert(vaoCube > 0);
    glBindVertexArray(vaoCube);
    cout << "vaoCube = " << vaoCube << endl;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert(glGetError() == GL_NONE);
    glGenBuffers(1, &cube.gVertexAttribBuffer);
    glGenBuffers(1, &cube.gIndexBuffer);
    assert(cube.gVertexAttribBuffer > 0 && cube.gIndexBuffer > 0);
    glBindBuffer(GL_ARRAY_BUFFER, cube.gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  cube.gIndexBuffer);
    cube.gVertexDataSizeInBytes =  cube.gVertices.size() * 3 * sizeof(GLfloat); 
    cube.gNormalDataSizeInBytes =  cube.gNormals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytesC =  cube.gFaces.size() * 3 * sizeof(GLuint);
    GLfloat* vertexDataC = new GLfloat[ cube.gVertices.size() * 3];
    GLfloat* normalDataC = new GLfloat[ cube.gNormals.size() * 3];
    GLint* indexDataC = new GLint[ cube.gFaces.size() * 3];
    for(int i = 0; i < cube.gVertices.size(); ++i) {
        vertexDataC[3 * i] = cube.gVertices[i].x;
        vertexDataC[3 * i + 1] = cube.gVertices[i].y;
        vertexDataC[3 * i + 2] = cube.gVertices[i].z;
    }
    for(int i = 0; i < cube.gNormals.size(); ++i) {
        normalDataC[3 * i] = cube.gNormals[i].x;
        normalDataC[3 * i + 1] = cube.gNormals[i].y;
        normalDataC[3 * i + 2] = cube.gNormals[i].z;
    }
    for(int i = 0; i < cube.gFaces.size(); ++i) {
        indexDataC[3 * i] = cube.gFaces[i].vIndex[0];
        indexDataC[3 * i + 1] = cube.gFaces[i].vIndex[1];
        indexDataC[3 * i + 2] = cube.gFaces[i].vIndex[2];
    }
    glBufferData(GL_ARRAY_BUFFER, cube.gVertexDataSizeInBytes + cube.gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cube.gVertexDataSizeInBytes, vertexDataC);
    glBufferSubData(GL_ARRAY_BUFFER, cube.gVertexDataSizeInBytes, cube.gNormalDataSizeInBytes, normalDataC);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexDataSizeInBytesC, indexDataC, GL_STATIC_DRAW);
    // done copying to GPU memory; can free now from CPU memory
    delete[] vertexDataC;
    delete[] normalDataC;
    delete[] indexDataC;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube.gVertexDataSizeInBytes));
}

void initFonts(int windowWidth, int windowHeight)
{
    // Set OpenGL options
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 0.0f, static_cast<GLfloat>(windowHeight));
    glUseProgram(textProgram);
    glUniformMatrix4fv(glGetUniformLocation(textProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSerif-Italic.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
                );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    //
    // Configure VBO for texture quads
    //
    glGenBuffers(1, &gTextVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        activeProgramIndex = 0;
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        activeProgramIndex = 1;
    }
    else if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        glShadeModel(GL_FLAT);
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        glShadeModel(GL_SMOOTH);
    }
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if ( key == GLFW_KEY_A && action == GLFW_PRESS){
        moveLeft = true;
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE){
        moveLeft = false;
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS){
        moveRight = true;
    }
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE){
        moveRight = false;
    }
    else if ( key == GLFW_KEY_R && action == GLFW_PRESS){
        restartState = true;
    }
    else if ( key == GLFW_KEY_R && action == GLFW_RELEASE){
        restartState = false;
    }
}

void init()
{
    //ParseObj("armadillo.obj");
    ParseObj("hw3_support_files/bunny.obj");
    ParseQuad("hw3_support_files/quad.obj");
    ParseCube("hw3_support_files/cube.obj");
    loadSkyTexture("hw3_support_files/sky.jpg");
    glEnable(GL_DEPTH_TEST);
    initShaders();
    initVBO();
    initFonts(width, height);
    cubeRand();

    std::cout << "bunny shader initialized" <<std::endl; 

    std::cout << "ground shader initialized" <<std::endl;
}

void drawBunnyModel()
{
    
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, bunny.gFaces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void drawGroundModel()
{
    glBindVertexArray(vaoG);
    glDrawElements(GL_TRIANGLES, quad.gFaces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void drawSkyModel()
{
    glDepthMask(GL_FALSE);  // Disable depth write

    glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, skyTexture);
    glUniform1i(skyTextureLocation, 0); // The 0 here corresponds to GL_TEXTURE0

    glBindVertexArray(vaoSky);
    glDrawElements(GL_TRIANGLES, sky.gFaces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);   // Re-enable depth write
}

void bunnyJump()
{
    if(!pause){
        bunny.positionY += bunny.velocityY;
        bunny.velocityY += gravity;
        if(bunny.positionY  <= 0.0)
        {
            bunny.positionY  = 0.0;
            bunny.velocityY = bunny.jumpVelocity;
        }
    }
}
void bunnymove(int direction){
    bunny.positionX += bunny.velocityX*direction;
    if(bunny.positionX >= 1.37)
    {
        bunny.positionX = 1.37;
    }
    else if(bunny.positionX <= -1.37)
    {
        bunny.positionX = -1.37;
    }
}
void bunnycheck(bool press, int direction){
    if(press){  
         if(direction > 0 ){
            bunnymove(1);
         }
        else if(direction < 0){
                bunnymove(-1);
     }
     else{
        bunnymove(0);
     }
    }

}
void happyBunny()
{
    happy = true;
}
void killBunny()
{
    faint = true;
    pause = true;
    std::cout << "dead bunny"<< std::endl;
}
void gameStop()
{
    gameSpeed = 0;
    bunny.velocityX = 0;
    bunny.velocityY = 0;
    moveLeft = 0;
    moveRight = 0;
} 
void displayBunny() {
    
    bunnyJump();
    bunnycheck(moveLeft,-1); // left -1
    bunnycheck(moveRight,1); // right 1

    // Compute the modeling matrix (transformation matrix for the object)
    glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-90. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0)); // Rotation around Y
    glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, -1.0, 2.8)); // Translation
    glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(0.15, 0.15, 0.15));      // Scaling
    if(faint || finished)
    {
        gameStop();
        bunny.angleX += 5;
        std::cout<<"fallen"<<std::endl;
        std::cout<<bunny.angleX<<std::endl;
        float faintAngleRad = (float)(bunny.angleX / 180.0) * M_PI;
        glm::mat4 matRfaint = glm::rotate(glm::mat4(1.0), faintAngleRad, glm::vec3(1.0, 0.0, 0.0)); // Rotation around X
        matR = matR * matRfaint;

        if (bunny.angleX >= 90)
        {
            bunny.angleX = 90;
            faint = false;
            finished = true;
            std::cout<<"finish"<<std::endl;

        }

    }

    else if(happy)
    {
        bunny.angleY += 30 * gameSpeed;
        float happyAngleRad = (float)(bunny.angleY / 180.0) * M_PI;
        glm::mat4 matRhappy = glm::rotate(glm::mat4(1.0), happyAngleRad, glm::vec3(0.0, 1.0, 0.0)); // Rotation around Y
        matR = matR * matRhappy;
        if(bunny.angleY>=360)
        {
            happy = false;
            bunny.angleY = 0;
        }
    }

    //Translation -> Rotation -> Scaling
    glm::mat4 matMove = glm::translate(glm::mat4(1.0), glm::vec3(bunny.positionX, 0.0, 0.0)); //bunny move
    glm::mat4 matThop = glm::translate(glm::mat4(1.0), glm::vec3(0.f, bunny.positionY, -4.0)); //bunny hop
    matT = matT*matMove*matThop;
    modelingMatrix = matT* matR * matS ; // Combine transformations
    /*  */
    // Set the active shader program and update its uniform variables
    glUseProgram(gProgram[activeProgramIndex]);
    glUniformMatrix4fv(projectionMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(viewingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    glUniformMatrix4fv(modelingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
    glUniform3fv(eyePosLoc[activeProgramIndex], 1, glm::value_ptr(eyePos));
    // Draw the model
    drawBunnyModel();
}
void displayQuad(){
    
	float angleRad = (float)(10 / 180.0) * M_PI;
   
	// Compute the modeling matrix 
	glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, -2.f, -2.f));
	glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(3.0, 1.0, 1000.0));
	glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-180. / 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(1.0, 0.0, 0.0));
	groundModelingMatrix = matT * matS * matR * matRz; // starting from right side, rotate around Y to turn back, then rotate around Z some more at each frame, then translate.

	
    glUseProgram(groundProgram);

    // Set the uniform values
    //GLint color1Location = glGetUniformLocation(groundProgram, "color1");
    //make color1 fuchsia
    glUniform3f(color1Location, 1.f,0.f,0.6f);
    //GLint color2Location = glGetUniformLocation(groundProgram, "color2");
    //make color2 turquoise
    glUniform3f(color2Location, 0.f,1.f,0.8f);
    //GLint scaleLocation = glGetUniformLocation(groundProgram, "scale");
    glUniform1f(scaleLocation,0.66);

    //GLint offsetLocation = glGetUniformLocation(groundProgram, "offset");

    glUniform3f(offsetLocation,offset.x,offset.y,offset.z);
    glUniformMatrix4fv(groundProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(groundViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    glUniformMatrix4fv(groundModelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(groundModelingMatrix));
    glUniform3fv(groundEyePosLoc, 0.7, glm::value_ptr(eyePos));
	// Draw the scene

	drawGroundModel();
    offset.z -= gameSpeed;
    gameSpeed += gameAcceleration;
    if(!pause){    score += 0.2;
}

}
void displaySky(){
    float angleRad = (float)(90 / 180.0) * M_PI;

    // Compute the modeling matrix
    glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, -1.f));
    glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(2.0, 1.0, 1.0));
    glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (90. / 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(1.0, 0.0, 0.0));
    skyModelingMatrix = matT * matS; // starting from right side, rotate around Y to turn b

    glUseProgram(skyProgram);

    glUniformMatrix4fv(skyProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(skyViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    glUniformMatrix4fv(skyModelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(skyModelingMatrix));
    glUniform3fv(skyEyePosLoc, 1, glm::value_ptr(eyePos));

    drawSkyModel();
}

void drawCube(){
    glDrawElements(GL_TRIANGLES, cube.gFaces.size() * 3, GL_UNSIGNED_INT, 0);
}

void displayCube(){
    float angleRad = (float)(10 / 180.0) * M_PI;
    glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(5, -2.f, -100.f));
	glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(.5, 5.0,0.3));
	glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-180. / 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(1.0, 0.0, 0.0));

	cubeModelingMatrix = matT * matS ; // starting from right side, rotate around Y to turn back, then rotate around Z some more at each frame, then translate.

    for(int i =0 ; i<3 ; i++){
     
        cubeModelingMatrix = glm::translate(cubeModelingMatrix, glm::vec3(-5.f, 0.f, 0.f));

        if(colorRandomizer[i] == 0){
        //red
            glUniform3f(cubeColor1Location, 1.f,0.f,0.1f);
        }
        else{
            //make it yellow
            glUniform3f(cubeColor1Location, 1.f,1.f,0.1f);
        }
        if(hittenCube == i){
            continue;
        }
        
        glUniform3f(cubeLightPosLocation, 5.f,5.f,5.f);
        glUniform3f(cubeLightColorLocation, 1.f,1.f,1.f);
        glUniform1f(cubeScaleLocation,1);
        glUniform3f(cubeOffsetLocation,Coffset.x,Coffset.y,Coffset.z);
        glUniformMatrix4fv(cubeProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(cubeViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewingMatrix));
        glUniformMatrix4fv(cubeModelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(cubeModelingMatrix));
        glUniform3fv(cubeEyePosLoc, 0.7, glm::value_ptr(eyePos));
        drawCube();
        
        if(Coffset.z >= 96.8 && hitten == false){
            if( bunny.positionX+0.14 >= 1.00 && bunny.positionX-0.14 <= 1.50){
                if(colorRandomizer[0] == 0){
                    //red
                    killBunny();
                    hittenCube = 0;
                    hitten = true;
                    std::cout<<"redcollusion"<<std::endl;
                    }
                else{
                   happyBunny();
                    std::cout<<"yellowc"<<std::endl;

                    score  += 1000;
                    //gameSpeed += 0.04;
                    //bunny.velocityX += 0.005;
                    Coffset.z = -2 ;
                    cubeRand();
                

                }
            }
            else if(bunny.positionX+0.14 >= -0.25 && bunny.positionX-0.14 <= 0.25){
                ///
                if(colorRandomizer[1] == 0){
                    killBunny();
                    hittenCube = 1;
                    hitten = true;
                    std::cout<<"redcollusion"<<std::endl;
                }
                else{
                    //yellow
                    happyBunny();
                    std::cout<<"yellowc"<<std::endl;
                    score  += 1000;
                    
                    //gameSpeed += 0.04;
                    //bunny.velocityX += 0.005;
                    Coffset.z = -2 ;
                    cubeRand();
                    

                }
            }
            else if(bunny.positionX+0.14 >= -1.50 && bunny.positionX-0.14 <= -1.00){
                if(colorRandomizer[2] == 0){
                    killBunny();
                    hittenCube = 2;
                    hitten = true;
                    std::cout<<"redcollusion"<<std::endl;
                }
                else{
                    //yellow
                    happyBunny();
                    std::cout<<"yellowc"<<std::endl;
                    //gameSpeed += 0.04;
                    score  += 1000;
                    //bunny.velocityX += 0.005;
                    Coffset.z = -2 ;
                    cubeRand();
                }
            }
            else{
                if(Coffset.z >= 99){
                    Coffset.z = -2 ;
                    cubeRand();
                }
            }
        }

    }
    Coffset.z += gameSpeed;

}
void restart(){
    hittenCube = -1;
    hitten = false;
    score = 0;
    gravity = -0.0025;
    bunny.velocityX = moveSpeed;
    bunny.positionX = 0;
    bunny.positionY = 0;
    bunny.velocityY = 0;
    finished= false;
    pause = false;
    happy = false;
    faint = false;
    gameSpeed = 0.2;
    offset.z = 0;
    Coffset.z = 0;
    moveRight = false;
    moveLeft = false;
    cubeRand();

}

void display()
{
    if(restartState){
        restart();
    }
    bunny.velocityX += 0.00004;
     gravity -= 0.0000005;
    gameSpeed += 0.00006;
    glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    displaySky();
    displayBunny();
    displayQuad();
    glUseProgram(cubeProgram);
    glBindVertexArray(vaoCube);
    displayCube();
    std::string text = "Score: " + std::to_string(int(score));
    if(faint || finished){
    renderText( text , 25.0f, 630.0f, 1.0f, glm::vec3(1.0, 0.f, 0.f));}
    else{
        renderText( text , 25.0f, 630.0f, 1.0f, glm::vec3(1., 1.f, 0.f));
    }
    glBindVertexArray(0);
    
}

void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    glViewport(0, 0, w, h);

    // Use perspective projection
    float fovyRad = (float)(90.0 / 180.0) * M_PI;
    projectionMatrix = glm::perspective(fovyRad, w / (float)h, 1.0f, 100.0f);

    // Assume default camera position and orientation (camera is at
    // (0, 0, 0) with looking at -z direction and its up vector pointing
    // at +y direction)
    //
    //viewingMatrix = glm::mat4(1);
    viewingMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) + glm::vec3(0, 0, -1), glm::vec3(0,2, 0));
    //lookAt(eye, center, upvector)

}


// Function to run the main rendering loop
void mainLoop(GLFWwindow* window) {
    // Loop until the window is instructed to close

    while (!glfwWindowShouldClose(window)) {
        // Call the display function to render the scene
        display();

        // Swap the front and back buffers
        // GLFW uses double buffering to avoid flickering and tearing artifacts
        glfwSwapBuffers(window);

        // Poll for and process events like keyboard and mouse input
        glfwPollEvents();
    }
}

int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
    std::cout << "line 819" << std::endl;
    GLFWwindow* window;
    if (!glfwInit())
    {
        exit(-1);
    }

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this if on MacOS
    std::cout << "line 833" << std::endl;
     width = 1000;
      height = 800;
    window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    std::cout << "line 845" << std::endl;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    char rendererInfo[512] = { 0 };
    strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER)); // Use strcpy_s on Windows, strcpy on Linux
    strcpy(rendererInfo, " - "); // Use strcpy_s on Windows, strcpy on Linux
    strcpy(rendererInfo, (const char*)glGetString(GL_VERSION)); // Use strcpy_s on Windows, strcpy on Linux
    glfwSetWindowTitle(window, rendererInfo);
    init();

    glfwSetKeyCallback(window, keyboard);
    glfwSetWindowSizeCallback(window, reshape);

    reshape(window, width, height); // need to call this once ourselves
    mainLoop(window); // this does not return unless the window is closed

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}