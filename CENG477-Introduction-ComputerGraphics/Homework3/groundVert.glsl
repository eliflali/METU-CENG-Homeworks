#version 330 core

// Uniform variables passed from the OpenGL application
uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;

// Vertex attributes (input from VBO)
layout(location=0) in vec3 inVertex; // Vertex position
layout(location=1) in vec3 inNormal; // Vertex normal

// Output to the fragment shader
out vec4 fragWorldPos; // World position of the vertex
out vec3 fragWorldNor; // World normal of the vertex

void main(void) {
    // Compute the world coordinates of the vertex
    fragWorldPos = modelingMatrix * vec4(inVertex, 1);
    // Compute the transformation of the normal to world coordinates
    fragWorldNor = inverse(transpose(mat3x3(modelingMatrix))) * inNormal;

    // Transform the vertex position to clip space
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1);
}


