#include "kk_headers.h"
#include <GL/glut.h>

// 전역 변수 선언
extern float angleX, angleY;

void drawBody()
{
    GLUquadric *quad = gluNewQuadric();

    // 몸통 그리기
    glPushMatrix();
    glTranslatef(0.0f, -0.3f, 0.0f); // 화면 중앙보다 살짝 아래
    glScalef(0.8f, 1.1f, 0.5f);      // 타원형 몸통 느낌 (좀 더 넓적하게)
    glColor3f(1.0f, 1.0f, 1.0f);     // 하얀색 (K.K.는 흰색)
    gluSphere(quad, 0.8f, 30, 30);   // 구체로 몸통 생성
    glPopMatrix();

    // 몸통 하단에 네모난 부분 추가
    glPushMatrix();
    glTranslatef(0.0f, -0.9f, 0.0f); // 몸통 하단으로 이동
    glScalef(0.5f, 0.3f, 0.3f);      // 네모난 느낌
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCube(1.0f); // 큐브로 하단 표현
    glPopMatrix();

    // 왼쪽 어깨 그리기
    glPushMatrix();
    glTranslatef(-0.50f, 0.295f, 0.0f); // 위치 조정 (약간 위로)
    glScalef(0.3f, 0.2f, 0.25f);        // Z축으로 더 납작하게 변경
    glColor3f(1.0f, 1.0f, 1.0f);        // 하얀색
    gluSphere(quad, 0.6f, 20, 20);      // 구형 어깨
    glPopMatrix();

    // 왼쪽 어깨 연결부
    glPushMatrix();
    glTranslatef(-0.51f, 0.27f, 0.0f);             // 어깨 위치에서 시작 (0.02f 위로)
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);            // Y축으로 90도 회전
    glColor3f(1.0f, 1.0f, 1.0f);                   // 하얀색
    gluCylinder(quad, 0.12f, 0.12f, 0.2f, 20, 20); // 연결부 원통
    glPopMatrix();

    // 오른쪽 어깨 그리기
    glPushMatrix();
    glTranslatef(0.50f, 0.295f, 0.0f); // 위치 조정 (약간 위로)
    glScalef(0.3f, 0.2f, 0.25f);       // Z축으로 더 납작하게 변경
    glColor3f(1.0f, 1.0f, 1.0f);       // 하얀색
    gluSphere(quad, 0.6f, 20, 20);     // 구형 어깨
    glPopMatrix();

    // 오른쪽 어깨 연결부
    glPushMatrix();
    glTranslatef(0.51f, 0.27f, 0.0f);              // 어깨 위치에서 시작 (0.02f 위로)
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);           // Y축으로 -90도 회전
    glColor3f(1.0f, 1.0f, 1.0f);                   // 하얀색
    gluCylinder(quad, 0.12f, 0.12f, 0.2f, 20, 20); // 연결부 원통
    glPopMatrix();

    // 왼쪽 팔 그리기
    glPushMatrix();
    glTranslatef(-0.55f, 0.32f, 0.0f);             // 몸통 왼쪽으로 이동 (약간 위로)
    glRotatef(95.0f, 1.0f, 0.0f, 0.0f);            // 팔을 아래로 향하게
    glRotatef(10.0f, 0.0f, -15.0f, 1.0f);          // 팔을 Z축으로 15도 꺾기
    glColor3f(1.0f, 1.0f, 1.0f);                   // 하얀색
    gluCylinder(quad, 0.12f, 0.22f, 1.2f, 20, 20); // 더 얇은 원통형 팔
    // 팔 끝에 구 추가
    glTranslatef(0.0f, 0.0f, 1.2f); // 팔 끝으로 이동
    gluSphere(quad, 0.22f, 20, 20); // 더 작은 구형 손
    glPopMatrix();

    // 오른쪽 팔 그리기
    glPushMatrix();
    glTranslatef(0.55f, 0.32f, 0.0f);              // 몸통 오른쪽으로 이동 (약간 위로)
    glRotatef(95.0f, 1.0f, 0.0f, 0.0f);            // 팔을 아래로 향하게
    glRotatef(-10.0f, 0.0f, -15.0f, 1.0f);         // 팔을 Z축으로 -15도 꺾기 (오른쪽 팔은 반대 방향)
    glColor3f(1.0f, 1.0f, 1.0f);                   // 하얀색
    gluCylinder(quad, 0.12f, 0.22f, 1.2f, 20, 20); // 더 얇은 원통형 팔
    // 팔 끝에 구 추가
    glTranslatef(0.0f, 0.0f, 1.2f); // 팔 끝으로 이동
    gluSphere(quad, 0.22f, 20, 20); // 더 작은 구형 손
    glPopMatrix();

    gluDeleteQuadric(quad);
}

// void display()
// {
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//     glLoadIdentity();

//     // 카메라 위치 세팅
//     gluLookAt(0, 0, 3.0, // 카메라 위치
//               0, 0, 0,   // 바라보는 지점
//               0, 1, 0);  // 위쪽 방향

//     glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

//     // 회전 적용
//     glRotatef(angleX, 1.0f, 0.0f, 0.0f); // X축 회전
//     glRotatef(angleY, 0.0f, 1.0f, 0.0f); // Y축 회전

//     drawBody(); // 몸통 그리기

//     glutSwapBuffers();
// }
