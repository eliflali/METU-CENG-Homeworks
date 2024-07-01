# BunnyRun

## Introduction

BunnyRun is a 3D computer game developed using OpenGL and GLFW for rendering, and GLM for mathematical operations. The game features a bunny character navigating through a dynamic world, interacting with various objects and environments. The player controls the bunny, making it jump and move to avoid obstacles and collect rewards.

## System Requirements

* OpenGL 3.3 or higher
* GLFW library
* GLM (OpenGL Mathematics)
* GLEW (OpenGL Extension Wrangler Library)
* FreeType library for font rendering
* C++ compiler with C++11 support

## Game Structure

* ### Main Game Components:
* Bunny: The main character of the game.
* Ground: The surface on which the bunny moves.
* Cube: Obstacles that the bunny must avoid.
* Sky: The background environment.
* ### Key Classes and Functions:
* Character, Vertex, Texture, Normal, Face: Structures for managing object properties.
* ParseObj, ParseQuad, ParseCube: Functions for parsing object files.
* createVS, createFS: Functions for creating vertex and fragment shaders.
* initShaders, initVBO, initFonts: Functions for initializing shaders, vertex buffer objects, and fonts.
* displayBunny, displayQuad, displaySky, displayCube: Functions for rendering game components.
* keyboard: Function for handling keyboard input.

## How to Play

* ### Controls:
* A: Move left
* D: Move right
* R: Restart the game
* Q: Quit the game
  
* ### Gameplay:
* The bunny starts on the ground. Use left and right keys to avoid cubes.
* The game's difficulty increases over time with increased speed.
* Avoid red cubes and aim for yellow cubes to score points.





https://github.com/eliflali/CENG477-Homework3/assets/63200204/31d8f663-7bec-4b80-b274-ee0ca7ff8e32


