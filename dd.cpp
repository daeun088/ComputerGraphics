#include <GL/freeglut.h>
#include <iostream>

void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // 배경색을 검정색으로 설정
    glMatrixMode(GL_PROJECTION);             // 투영 행렬 모드 설정
    glLoadIdentity();                        // 투영 행렬 초기화
    gluOrtho2D(0, 500, 0, 500);             // 2D 좌표 시스템 설정
    glMatrixMode(GL_MODELVIEW);              // 모델뷰 행렬 모드 설정
    glLoadIdentity();                        // 모델뷰 행렬 초기화
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);  // 화면 지우기

    // 삼각형을 그리기
    glBegin(GL_TRIANGLES);  // 삼각형 그리기 시작
    glColor3f(1.0f, 0.0f, 0.0f);  // 빨간색
    glVertex2f(250.0f, 400.0f);  // 삼각형의 첫 번째 꼭짓점

    glColor3f(0.0f, 1.0f, 0.0f);  // 초록색
    glVertex2f(150.0f, 250.0f);  // 삼각형의 두 번째 꼭짓점

    glColor3f(0.0f, 0.0f, 1.0f);  // 파란색
    glVertex2f(350.0f, 250.0f);  // 삼각형의 세 번째 꼭짓점
    glEnd();  // 삼각형 그리기 끝

    glFlush();  // OpenGL 명령 실행
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Triangle on Black Background");

    // OpenGL 초기화
    initOpenGL();

    glutDisplayFunc(display);  // 화면 그리기 함수 등록
    glutMainLoop();  // 이벤트 루프 시작

    return 0;
}

