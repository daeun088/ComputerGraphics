#include "tree.h"
#include <GL/glut.h>
#include <cmath>
#include "stb_image.h"

// loadTexture 함수 선언을 가장 위에!
extern GLuint loadTexture(const char *filename);

// 잎 텍스처 전역 변수 선언
GLuint leafTexture = 0;

// 기둥/뿌리 텍스처 전역 변수
GLuint barkTexture = 0;

// 잎 텍스처 초기화 함수 (최초 1회만 호출)
void initLeafTexture()
{
    if (leafTexture == 0)
    {
        leafTexture = loadTexture("Stylized_Leaves_002_basecolor.jpg");
    }
}

// 기둥/뿌리 텍스처 초기화 함수
void initBarkTexture()
{
    if (barkTexture == 0)
    {
        barkTexture = loadTexture("Wood_Bark_006_basecolor.jpg");
    }
}

void drawTree()
{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING); // 조명 비활성화

    glPushMatrix();
    glTranslatef(0.0f, -0.7f, 0.0f); // 전체 나무를 아래로 이동
    glScalef(2.3f, 2.3f, 2.3f);

    // 1. 몸통(기둥) - 텍스처 적용
    initBarkTexture();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barkTexture);
    GLUquadric *barkQuad = gluNewQuadric();
    gluQuadricTexture(barkQuad, GL_TRUE);
    glColor3f(1, 1, 1);

    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(barkQuad, 0.2, 0.13, 1.0, 32, 32);
    glPopMatrix();

    // 1. 기둥 밑에 반구 추가
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barkTexture);
    GLUquadric *baseQuad = gluNewQuadric();
    gluQuadricTexture(baseQuad, GL_TRUE);
    glColor3f(1, 1, 1);
    glRotatef(-90, 1, 0, 0);
    gluSphere(baseQuad, 0.2, 32, 16); // 기둥 반지름과 맞추기
    gluDeleteQuadric(baseQuad);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    gluDeleteQuadric(barkQuad);
    glDisable(GL_TEXTURE_2D);

    // 2. 뿌리 개수와 윗반지름 조정
    for (int i = 0; i < 6; ++i)
    {
        float angle = i * 60.0f;
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, barkTexture);
        GLUquadric *rootQuad = gluNewQuadric();
        gluQuadricTexture(rootQuad, GL_TRUE);
        glColor3f(1, 1, 1);

        float x = 0.21f * cos(angle * M_PI / 180.0f);
        float z = 0.21f * sin(angle * M_PI / 180.0f);
        glTranslatef(x, -0.08f, z);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(rootQuad, 0.13, 0.0, 0.1, 16, 4);
        gluDeleteQuadric(rootQuad);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    // 3. 잎 텍스처 매핑
    initLeafTexture();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, leafTexture);
    GLUquadric *leafQuad = gluNewQuadric();
    gluQuadricTexture(leafQuad, GL_TRUE);
    glColor3f(1, 1, 1); // 텍스처 색상 그대로

    float leafY = 1.5f;
    float leafR = 0.38f;
    float leafYOffset = leafY - 0.5f;

    // 상단 1개
    glPushMatrix();
    glTranslatef(0.0f, leafY, 0.0f);
    gluSphere(leafQuad, 0.45, 32, 32);
    glPopMatrix();

    // 아래 2개
    glPushMatrix();
    glTranslatef(leafR, leafYOffset, 0.0f);
    gluSphere(leafQuad, 0.45, 32, 32);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-leafR, leafYOffset, 0.0f);
    gluSphere(leafQuad, 0.45, 32, 32);
    glPopMatrix();

    gluDeleteQuadric(leafQuad);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glPopAttrib();               // 조명 상태 복원
    glColor3f(1.0f, 1.0f, 1.0f); // 색상 상태 복원
}