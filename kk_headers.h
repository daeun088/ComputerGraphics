#ifndef KK_HEADERS_H
#define KK_HEADERS_H

#include <GL/glut.h>

// Background functions
void drawSkybox();
void drawGround();

// Tree functions
void drawTree();

// Guitar functions
void drawGuitar();
void generateNeckEndPositions();
void loadAllTextures();
GLuint loadTexture2(const char *filename);

// Character functions
void drawKKHead();
void drawFootWithToes(float xOffset);
void drawBody();

#endif