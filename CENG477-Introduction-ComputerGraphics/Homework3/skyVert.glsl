#version 330 core

// Uniform variables passed from the OpenGL application
uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;

// Vertex attributes (input from VBO)
layout(location=0) in vec3 inVertex; // Vertex position
layout(location=1) in vec3 inNormal; // Vertex normal
layout(location=2) in vec2 texCoord;
out vec2 TexCoord;

void main(void) {
    TexCoord = texCoord;
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1);
}


