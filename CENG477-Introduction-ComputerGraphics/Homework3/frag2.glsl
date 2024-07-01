#version 330 core

uniform vec3 eyePos;

in vec4 fragWorldPos;
in vec3 fragWorldNor;

out vec4 fragColor;

// Light and material properties
vec3 I = vec3(1, 1, 1);          // Point light intensity
vec3 Iamb = vec3(0.8, 0.8, 0.8); // Ambient light intensity
vec3 lightPos = vec3(5, 5, 5);   // Light position in world coordinates

// Golden material properties
vec3 kd = vec3(1.0, 0.843, 0.0); // Diffuse reflectance (golden)
vec3 ka = vec3(0.3, 0.25, 0.1);  // Ambient reflectance (slightly golden)
vec3 ks = vec3(0.9, 0.9, 0.6);   // Specular reflectance (to add shine)
float shininess = 50.0;          // Shininess coefficient

void main(void)
{
	// Compute lighting
	vec3 L = normalize(lightPos - vec3(fragWorldPos));
	vec3 V = normalize(eyePos - vec3(fragWorldPos));
	vec3 H = normalize(L + V);
	vec3 N = normalize(fragWorldNor);

	float NdotL = dot(N, L); // for diffuse component
	float NdotH = dot(N, H); // for specular component

	vec3 diffuseColor = I * kd * max(0, NdotL);
	vec3 specularColor = I * ks * pow(max(0, NdotH), shininess);
	vec3 ambientColor = Iamb * ka;

	fragColor = vec4(diffuseColor + specularColor + ambientColor, 1);
}
