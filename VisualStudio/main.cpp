#include <windows.h>
#include "kk_headers.h"
#include <glut.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

int windowWidth, windowHeight;
int th, ph;
double dim;
int fov;
double asp;
double ecX, ecY, ecZ;

float angleX = 0, angleY = 0, zoom = -5.0;
int prevX, prevY;
bool isDragging = false;
GLuint eyeTexture;

void display();
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);


void initLighting()
{
    GLfloat pos[] = {0.0f, 3.5f, 1.3f, 0.0f};
    GLfloat amb[] = {0.7f, 0.7f, 0.7f, 1.7f};
    GLfloat diff[] = {0.2f, 0.2f, 0.2f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    return texture;
}

float markerX = 0, markerY = 0, markerZ = 0;
bool showMarker = false;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    drawSkybox();

    glLoadIdentity();
    glTranslatef(0, 0, zoom);
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);

    drawGround();

    drawKKHead();
    drawBody();

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

    glPushMatrix();
    glTranslatef(1.5f, 0.1f, 0.0f); 
    glScalef(2.25f, 2.25f, 2.25);
    drawGuitar();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.5f, -1.0f, -2.5f);
    drawTree();
    glPopMatrix();

    glutSwapBuffers();
}

void mouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
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

void keyboard(unsigned char key, int x, int y)
{
    if (key == 'a')
    {
        zoom += 0.3f;
    }
    else if (key == 'z')
    {
        zoom -= 0.3f;
    }
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80, (double)width / height, 1.0, 1000.0);
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
    eyeTexture = loadTexture("Image/kk_eye1.png");
    loadAllTextures();
    initLighting();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
