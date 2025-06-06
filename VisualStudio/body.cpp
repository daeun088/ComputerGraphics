#include <glut.h>

#include "kk_headers.h"

extern float angleX, angleY;

void drawBody() {
    GLUquadric* quad = gluNewQuadric();

    glPushMatrix();
    glTranslatef(0.0f, -0.3f, 0.0f);
    glScalef(0.8f, 1.1f, 0.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluSphere(quad, 0.8f, 30, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -0.9f, 0.0f);
    glScalef(0.5f, 0.3f, 0.3f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.50f, 0.295f, 0.0f);
    glScalef(0.3f, 0.2f, 0.25f);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluSphere(quad, 0.6f, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.51f, 0.27f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluCylinder(quad, 0.12f, 0.12f, 0.2f, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.50f, 0.295f, 0.0f);
    glScalef(0.3f, 0.2f, 0.25f);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluSphere(quad, 0.6f, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.51f, 0.27f, 0.0f);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluCylinder(quad, 0.12f, 0.12f, 0.2f, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.55f, 0.32f, 0.0f);
    glRotatef(95.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(10.0f, 0.0f, -15.0f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluCylinder(quad, 0.12f, 0.22f, 1.2f, 20, 20);
    glTranslatef(0.0f, 0.0f, 1.2f);
    gluSphere(quad, 0.22f, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.55f, 0.32f, 0.0f);
    glRotatef(95.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-10.0f, 0.0f, -15.0f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluCylinder(quad, 0.12f, 0.22f, 1.2f, 20, 20);

    glTranslatef(0.0f, 0.0f, 1.2f);
    gluSphere(quad, 0.22f, 20, 20);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

