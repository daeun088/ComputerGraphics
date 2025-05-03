#include <GL/glut.h>


float angle = 0.0;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 카메라 위치 설정
    gluLookAt(0.0, 0.0, 5.0,   // eye
              0.0, 0.0, 0.0,   // center
              0.0, 1.0, 0.0);  // up

    // 회전
    glRotatef(angle, 1.0, 1.0, 0.0);

    // 큐브 그리기
    glutSolidCube(1.0);

    glutSwapBuffers();
}

void idle() {
    angle += 0.1f;
    if (angle > 360) angle -= 360;
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / (float)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Cube Example");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
