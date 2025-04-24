// upper_body.cpp
#include <GL/glut.h>

void drawBody()
{
    GLUquadric *quad = gluNewQuadric();

    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 0.0f); // 화면 중앙보다 살짝 아래
    glScalef(1.0f, 1.3f, 0.7f);      // 타원형 몸통 느낌
    glColor3f(0.5f, 0.5f, 0.5f);     // 연한 회색

    gluSphere(quad, 0.7f, 30, 30); // 구체로 몸통 생성

    glPopMatrix();
    gluDeleteQuadric(quad);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 카메라 위치 세팅
    gluLookAt(0, 0, 3.0, // 카메라 위치
              0, 0, 0,   // 바라보는 지점
              0, 1, 0);  // 위쪽 방향

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    drawBody(); // 몸통 그리기

    glutSwapBuffers();
}
