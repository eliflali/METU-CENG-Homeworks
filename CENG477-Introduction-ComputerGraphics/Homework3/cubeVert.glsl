#version 330 core

layout (location = 0) in vec3 aPosition; // Vertex position (x, y, z)
layout (location = 1) in vec3 aNormal;   // Vertex normal (x, y, z)

out vec3 Normal;       // For passing to the fragment shader
out vec3 FragPos;      // For passing the fragment position to the fragment shader

uniform mat4 modelingMatrix;    // modelingMatrix matrix
uniform mat4 viewingMatrix;     // viewingMatrix matrix
uniform mat4 projectionMatrix; 
uniform vec3 offset; // Offset of the object
uniform float scale; // projectionMatrix matrix

void main()
{
    // Transform vertex position into homogeneous coordinates
    vec4 vertexPosition = modelingMatrix * vec4(aPosition, 1.0);
    vertexPosition = vec4(vertexPosition.x * scale + offset.x, vertexPosition.y * scale + offset.y, vertexPosition.z * scale + offset.z, 1.0);
    FragPos = vec3(vertexPosition); // Pass position to fragment shader
    
    // Transform normals and pass to fragment shader
    Normal = mat3(transpose(inverse(modelingMatrix))) * aNormal;
    
    // Project the position to clip space
    gl_Position = projectionMatrix * viewingMatrix * vertexPosition;
}
