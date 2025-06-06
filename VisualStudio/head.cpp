#include "kk_headers.h"
#include <glut.h>
#include <cmath>

extern GLuint eyeTexture;
extern float angleX, angleY, zoom;

void setColor(float r, float g, float b) { glColor3f(r, g, b); }

// 접힌 귀
void drawFoldedEar(float x, float y, float z, float angle)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0, 0, 1);
    glScalef(0.05, 0.06, 0.11);
    setColor(1.0, 1.0, 1.0);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();

    float num;
    if (x < 0)
    {
        num = -x;
    }
    else
    {
        num = x;
    }
    glPushMatrix();
    glTranslatef(1.18 * x, y + num - (1.35 * num), z);
    glRotatef(1.5 * angle, 0, 0, 1);
    glScalef(0.2, 0.06, 0.15);
    setColor(1.0, 1.0, 1.0);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();
}

void drawEyebrow(float x, float y, float z, float angle)
{
    glPushMatrix();
    if (x > 0)
    {
        glTranslatef(x - 0.061, y, z);
        glRotatef(30, 0, 1, 0);
        glTranslatef(-(x - 0.061), -y, -z);
    }
    else
    {
        glTranslatef(x + 0.061, y, z);
        glRotatef(-30, 0, 1, 0);
        glTranslatef(-(x + 0.061), -y, -z);
    }
    for (int i = 1; i < 7; i++)
    {
        glPushMatrix();
        glTranslatef(x, y + (0.01 * i), z);
        glRotatef(angle, 0, 0, 1);
        glScalef(0.15 - (0.016 * i), 0.02 + (0.002 * i), 0.025);
        setColor(0, 0, 0);
        glutSolidSphere(1.0, 10, 10);
        glPopMatrix();
    }
    float xOffset = (x > 0) ? -0.061 : +0.061;
    for (int i = 6; i > 0; i--)
    {
        glPushMatrix();
        glTranslatef(x + xOffset, y + (0.01 * i), z);
        glRotatef(-angle, 0, 0, 1);
        glScalef(0.15 - (0.016 * i), 0.02 + (0.003 * i), 0.025);
        setColor(0, 0, 0);
        glutSolidSphere(1.0, 10, 10);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawSnout()
{
    glPushMatrix();
    glTranslatef(0.0, -0.34, 0.2);
    glScalef(0.25, 0.18, 0.22);
    setColor(1.0, 1.0, 1.0);
    glutSolidSphere(1.0, 24, 24);
    glPopMatrix();
}

void drawNose()
{
    glPushMatrix();
    glTranslatef(0, -0.28, 0.40);
    glScalef(0.6, 0.4, 0.3);
    setColor(0, 0, 0);
    glutSolidSphere(0.135, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.04, -0.28, 0.418);
    setColor(1.0, 1.0, 1.0);
    glutSolidSphere(0.02, 12, 12);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawMouth()
{
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(0.033, -0.41, 0.40);
    glRotatef(60, 0.5, 0, 0.8);
    glScalef(0.02, 0.12, 0.005);
    setColor(0.5f, 0.5f, 0.5f);
    glutSolidCube(1.0);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(-0.033, -0.41, 0.40);
    glRotatef(-60, -0.5, 0, 0.8);
    glScalef(0.02, 0.12, 0.005);
    setColor(0.5f, 0.5f, 0.5f);
    glutSolidCube(1.0);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(0, -0.35, 0.42);
    glRotatef(0, 100, 100, 100);
    glScalef(0.02, 0.05, 0.005);
    setColor(0.5f, 0.5f, 0.5f);
    glutSolidCube(1.0);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawEyeOnHeadSphere(float xOffset, float yOffset, float zOffset)
{
    const float radius = 0.4f;
    const float thetaStart = 0.35f * M_PI;
    const float thetaEnd = 0.50f * M_PI;
    const float phiStart = 0.20f * M_PI;
    const float phiEnd = 0.80f * M_PI;
    const int slices = 60;
    const int stacks = 40;

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, eyeTexture);
    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();
    glTranslatef(xOffset, yOffset, zOffset);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    for (int i = 0; i < stacks; ++i)
    {
        float theta1 = thetaStart + (thetaEnd - thetaStart) * i / stacks;
        float theta2 = thetaStart + (thetaEnd - thetaStart) * (i + 1) / stacks;
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j)
        {
            float phi = phiStart + (phiEnd - phiStart) * j / slices;
            for (int k = 0; k < 2; ++k)
            {
                float theta = (k == 0) ? theta1 : theta2;
                float x = radius * sin(theta) * cos(phi);
                float y = radius * sin(theta) * sin(phi);
                float z = radius * cos(theta);
                float u = (phi - phiStart) / (phiEnd - phiStart);
                float v = 1.0f - (theta - thetaStart) / (thetaEnd - thetaStart);
                glTexCoord2f(u, v);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void drawKKHead()
{
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, 0.0f);
    glScalef(2.2, 2.2, 2.2);
    glEnable(GL_LIGHTING);
    GLUquadric *quad = gluNewQuadric();
    int num = -1;
    while (num > -128)
    {
        glPushMatrix();
        setColor(1.0, 1.0, 1.0);
        glTranslatef(0.0, 0.0018 * num, 0);
        glScalef(1.0, 1.0, 1.0);
        gluSphere(quad, 0.4 + 0.0009 * num, 64, 64);
        glPopMatrix();
        num--;
    }
    gluDeleteQuadric(quad);

    drawFoldedEar(-0.38, 0.2, -0.05, 50);
    drawFoldedEar(0.38, 0.2, -0.05, -50);

    drawEyebrow(-0.23, 0.01, 0.35, 20);
    drawEyebrow(0.23, 0.01, 0.35, -20);
    drawEyeOnHeadSphere(0.0f, -0.03f, -0.0f);

    drawSnout();
    drawNose();
    drawMouth();
    glPopMatrix();
}