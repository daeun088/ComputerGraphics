#include <GL/glut.h>

// 전역 변수 선언
int windowWidth, windowHeight;
int th, ph;
double dim;
int fov;
double asp;
double ecX, ecY, ecZ;
float angleX = 0.0f;     // X축 회전 각도
float angleY = 0.0f;     // Y축 회전 각도
int lastX, lastY;        // 마지막 마우스 위치
bool isDragging = false; // 드래그 중인지 여부

extern void display();

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;
    float aspect = (float)w / h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 1.0, 10.0);

    glMatrixMode(GL_MODELVIEW);
}

// 마우스 클릭 이벤트 처리
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            isDragging = true;
            lastX = x;
            lastY = y;
        }
        else
        {
            isDragging = false;
        }
    }
}

// 마우스 이동 이벤트 처리
void motion(int x, int y)
{
    if (isDragging)
    {
        angleY += (x - lastX) * 0.5f;
        angleX += (y - lastY) * 0.5f;
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("K.K. Concert!");

    // initGlobals();
    // initSkybox();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);   // 마우스 클릭 이벤트 등록
    glutMotionFunc(motion); // 마우스 이동 이벤트 등록
    glutMainLoop();
    return 0;
}
