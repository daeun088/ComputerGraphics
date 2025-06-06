#include "kk_headers.h"
#include <glut.h>
#include "stb_image.h"
#include <cmath>

extern GLuint loadTexture(const char *filename);

GLuint leafTexture = 0;

GLuint barkTexture = 0;

void initLeafTexture()
{
    if (leafTexture == 0)
    {
        leafTexture = loadTexture("Image/tree/leaf.jpg");
    }
}

void initBarkTexture()
{
    if (barkTexture == 0)
    {
        barkTexture = loadTexture("Image/tree/wood.jpg");
    }
}

void drawTree()
{
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(0.0f, -0.7f, 0.0f);
    glScalef(2.3f, 2.3f, 2.3f);

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

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, barkTexture);
    GLUquadric *baseQuad = gluNewQuadric();
    gluQuadricTexture(baseQuad, GL_TRUE);
    glColor3f(1, 1, 1);
    glRotatef(-90, 1, 0, 0);
    gluSphere(baseQuad, 0.2, 32, 16);
    gluDeleteQuadric(baseQuad);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    gluDeleteQuadric(barkQuad);
    glDisable(GL_TEXTURE_2D);

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

    initLeafTexture();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, leafTexture);
    GLUquadric *leafQuad = gluNewQuadric();
    gluQuadricTexture(leafQuad, GL_TRUE);
    glColor3f(1, 1, 1);

    float leafY = 1.5f;
    float leafR = 0.38f;
    float leafYOffset = leafY - 0.5f;

    glPushMatrix();
    glTranslatef(0.0f, leafY, 0.0f);
    gluSphere(leafQuad, 0.45, 32, 32);
    glPopMatrix();

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
    glPopAttrib();
    glColor3f(1.0f, 1.0f, 1.0f);
}