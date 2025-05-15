#include <GL/glut.h>
#include "body.h"
#include "head.h"
#include "leg.h"
#include "background.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>
#include <array>
#include <cmath>

// 전역 변수 선언
int windowWidth, windowHeight;
int th, ph;
double dim;
int fov;
double asp;
double ecX, ecY, ecZ;

// 회전/줌 상태
float angleX = 0, angleY = 0, zoom = -5.0; // zoom이 처음 시작 시점 위치
int prevX, prevY;
bool isDragging = false;
GLuint eyeTexture;

// 조명
void initLighting()
{
    GLfloat pos[] = {0.0f, 3.5f, 1.3f, 0.0f};  // 위쪽에서 약간 앞쪽 방향 (자연광 느낌)
    GLfloat amb[] = {0.7f, 0.7f, 0.7f, 1.7f};  // 주변광: 충분히 밝지만 너무 세지 않게
    GLfloat diff[] = {0.2f, 0.2f, 0.2f, 1.0f}; // 확산광: 입체감 살림

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);

    // 감쇠 최소화 → 거리에 관계없이 거의 균일하게
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.005f);
}

GLuint loadTexture(const char *filename)
{
    int width, height, channels;
    unsigned char *image = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if (!image)
    {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 텍스처 파라미터
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 이미지 업로드
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    return texture;
}

// 디스플레이 콜백
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawSkybox();

    glLoadIdentity();
    glTranslatef(0, 0, zoom);
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);

    drawGround();

    drawKKHead();
    drawBody(); // 몸통 그리기

    // 양쪽 다리 그리기
    glPushMatrix();
    glTranslatef(-0.3f, -1.85f, 0.1f);
    glRotatef(180, 0, 1, 0);
    glScalef(0.55f, 0.55f, 0.55f);
    drawFootWithToes(0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.3f, -1.85f, 0.1f);
    glRotatef(180, 0, 1, 0);
    glScalef(0.55f, 0.55f, 0.55f);
    drawFootWithToes(0.0f);
    glPopMatrix();

    glutSwapBuffers();
}
// 마우스 회전
void mouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        isDragging = (state == GLUT_DOWN);
        prevX = x;
        prevY = y;
    }
}

void mouseMotion(int x, int y)
{
    if (isDragging)
    {
        angleY += (x - prevX) * 0.5;
        angleX += (y - prevY) * 0.5;
        prevX = x;
        prevY = y;
        glutPostRedisplay();
    }
}
// 키보드 줌
void keyboard(unsigned char key, int x, int y)
{
    if (key == 'a')
        zoom += 0.3f;
    if (key == 'z')
        zoom -= 0.3f;
    glutPostRedisplay();
}

// 리사이즈
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80, (double)width / height, 1.0, 1000.0); //1000으로 수정!!중요!!작게하면 줌아웃시 캐릭터 사라짐
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("K.K. Concert!");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glClearColor(0.1, 0.5, 0.5, 1.0);
    eyeTexture = loadTexture("kk_eye1.png");
    initLighting();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
