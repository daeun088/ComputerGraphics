#ifndef GUITAR_H
#define GUITAR_H


#include <GL/glut.h>  // 또는 <OpenGL/gl.h> 와 <GLUT/glut.h> (macOS)
void drawGuitar();
void generateNeckEndPositions();
void loadAllTextures();  // 이건 내부적으로 loadTexture 쓸 거니까
GLuint loadTexture2(const char* filename);  // 선언만!

#endif