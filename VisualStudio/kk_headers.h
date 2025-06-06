#ifndef KK_HEADERS_H
#define KK_HEADERS_H

#include <windows.h>
#include <glut.h>
#include <array>
#include <vector>

// M_PI 정의 (일부 컴파일러에서 없을 수 있음)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 전역 변수 선언 (extern으로)
extern float angleX, angleY, zoom;

// Background functions
void drawSkybox();
void drawGround();

// Tree functions
extern GLuint leafTexture;
void drawTree();

// Guitar functions
void drawGuitar();
void generateNeckEndPositions();
void loadAllTextures();
std::array<float, 3> pegUVtoPosition(float u, float v);
GLuint loadTexture2(const char* filename);

// Character functions
void drawKKHead();
void drawFootWithToes(float xOffset);
void drawBody();

#endif