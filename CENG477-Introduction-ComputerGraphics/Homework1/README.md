# Raytracer 

Raytracer algorithm implemented as the first homework of Introduction to Graphics lecture, CENG 477. 

---

### raytracer.cpp:
#### clamp Function: 
This utility function ensures that the calculated color values for each pixel are within the valid range (0-255) and applies a rounding operation with a small epsilon value for precision.
#### Thread Structure: 
The Thread struct stores the data each thread needs to render a portion of the image, including start and end rows (startY, endY), image dimensions, the actual image buffer, camera, scene, and recursion depth.
#### executeThread Function:
This function, executed by each thread, generates rays for each pixel in its assigned portion of the image, computes the pixel color, and stores it in the image buffer.
### main Function:
  * #### Scene Parsing: 
  It starts by loading a scene from an XML file specified in argv[1].
  * #### Core Number Calculation: 
  Determines the number of processor cores available for parallel processing.
  * #### Thread Creation: 
  For each camera in the scene, the image buffer is allocated, and threads are created and assigned to render parts of the image. The height of the image is divided by the number of cores to distribute the workload evenly.
  * #### Thread Execution: 
  Each thread executes executeThread with its specific data. After all threads finish (ensured by pthread_join), the final image for the current camera is written to a file using     write_ppm.
  * #### Memory Management: 
  Dynamically allocated memory for the image buffer is released after processing each camera's view.

---

### computeColor.cpp:

#### computeSphereNormal: 
This function computes the normal vector of a sphere at a given point on its surface.
#### computeTriangleNormal:
Used for calculating the normal vector of a triangle, which can also be applied to mesh surfaces.
#### computeWi and computeWo: 
These functions calculate the incident light vector (wi) and the reflection vector (wo) at a point on a surface.
#### computeIrradiance: 
Calculates the light irradiance at a point on a surface, considering light intensity and distance from the light source.
#### computeHalfVector: 
Computes the half-vector used in specular reflection calculations, especially in the Phong reflection model.
#### computeAmbient, computeDiffuse, computeSpecular: 
These functions calculate the ambient, diffuse, and specular components of the light respectively, based on material properties and light characteristics.
#### checkShadow: 
Determines whether a point on a surface is in shadow relative to a light source.
#### isReflective and computeReflectionRay: 
These functions check if a material is reflective and compute the reflection ray if it is.
#### computePixelColor: 
The core function of this namespace, it calculates the color of a pixel by tracing a ray through the scene. It accounts for reflections, shadows, and light-material interactions, recursively handling reflections if necessary.

---

### intersectCalculator.cpp:

#### Function Definition: 
closestHit takes a Ray object, a reference to a HitPoint structure, the Scene, and two integers for ignoring specific objects. It returns a boolean indicating whether an intersection was found.
#### Variable Initialization:
* The function initializes variables to keep track of the number of spheres, meshes, and triangles in the scene.
* isHit is a boolean flag to indicate if any intersection is found.
* firstHit stores the nearest intersection distance found so far.
* currentHit temporarily holds the details of the latest intersection point found.
#### Sphere Intersection Check:
* Iterates over all spheres in the scene.
* For each sphere, it calculates the intersection time (hitTime) using the quadraticDelta function (from mathFunctions namespace).
* If a valid intersection is closer than any previously found intersection, it updates firstHit and currentHit with the new intersection details.
#### Triangle Intersection Check:
* Iterates over all triangles in the scene.
* Computes intersection using barycentric coordinates (alpha, beta, gamma) and checks if the intersection point is within the triangle.
* Updates firstHit and currentHit if a closer valid intersection is found.
#### Mesh Intersection Check:
* Iterates over all meshes and their faces (triangles).
* Similar to the triangle intersection, it checks for the closest intersection with any of the mesh faces.
* Updates firstHit and currentHit with details of the nearest intersection found.
#### Ignore Specific Object:
If the closest intersection found is with an object that should be ignored (as indicated by ignoreObjectId and ignoreObjectType), the function returns false.
#### Setting the Found HitPoint:
* If an intersection is found, currentHit is assigned to hitFound, which is passed by reference.
* The function returns true if an intersection is found, otherwise false.

---

### mathFunctions.cpp:

This C++ code snippet is part of a ray tracing application and it defines a collection of mathematical functions in the mathFunctions namespace, which are crucial for various calculations within the ray tracing process. 
