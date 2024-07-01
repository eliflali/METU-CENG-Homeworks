#version 330 core

in vec4 fragWorldPos; // From vertex shader
out vec4 color;

uniform float scale;
uniform vec3 offset;
uniform vec3 color1;
uniform vec3 color2;

void main() {
   bool x = int((fragWorldPos.x + offset.x+200) * scale) % 2 != 0;
    bool y = int((fragWorldPos.y + offset.y) * scale) % 2 != 0;
    bool z = int((fragWorldPos.z + offset.z) * scale) % 2 != 0;

    // Perform XOR operation on the boolean values
    bool xorXY = x != y;
    bool checkerPattern = xorXY != z;
    if (checkerPattern) {
        color = vec4(color1, 1.0f);
    } else {
        color = vec4(color2, 1.0f);
    }
}
