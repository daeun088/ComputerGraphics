#include "tree.h"
#include <GL/glut.h>
#include <cmath>

void drawTree()
{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING); // 조명 비활성화

    glPushMatrix();
    glTranslatef(0.0f, -0.70f, 0.0f); // 전체 나무를 아래로 이동

    glScalef(2.3f, 2.3f, 2.3f);

    // 1. 몸통(기둥) - 세로로 세우기
    glPushMatrix();
    glColor3f(0.8f, 0.6f, 0.2f); // 나무색
    GLUquadric *quad = gluNewQuadric();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, 0, 0);
    gluCylinder(quad, 0.2, 0.13, 1.0, 32, 32); // 아래가 더 굵은 원기둥
    glPopMatrix();

    // 2. 뿌리 6개
    for (int i = 0; i < 6; ++i)
    {
        float angle = i * 60.0f;
        glPushMatrix();
        glColor3f(0.8f, 0.6f, 0.2f);

        float x = 0.18f * cos(angle * M_PI / 180.0f);
        float z = 0.18f * sin(angle * M_PI / 180.0f);

        glTranslatef(x, -0.08f, z);
        glRotatef(-90, 1, 0, 0); // 세로로 세우기

        glutSolidCone(0.1, 0.1, 16, 4);

        glPopMatrix();
    }

    // 3. 잎 3덩이 (상단 1, 아래 2)
    float leafY = 1.5f;
    float leafR = 0.38f;

    // 상단 1개
    glPushMatrix();
    glColor3f(0.4f, 0.8f, 0.2f);
    glTranslatef(0.0f, leafY, 0.0f);
    glutSolidSphere(0.45, 32, 32);
    glPopMatrix();

    // 아래 2개
    float leafYOffset = leafY - 0.5f; // 아래로 조금 내림
    glPushMatrix();
    glColor3f(0.4f, 0.8f, 0.2f);
    glTranslatef(leafR, leafYOffset, 0.0f);
    glutSolidSphere(0.45, 32, 32);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.4f, 0.8f, 0.2f);
    glTranslatef(-leafR, leafYOffset, 0.0f);
    glutSolidSphere(0.45, 32, 32);
    glPopMatrix();

    glPopMatrix();

    glPopAttrib();               // 조명 상태 복원
    glColor3f(1.0f, 1.0f, 1.0f); // 색상 상태 복원
}