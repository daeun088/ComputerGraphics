#include "leg.h"
#include <GL/glut.h>
#include <vector>
#include <array>
#include <cmath>

static const std::array<std::array<float, 2>, 20> footCtrl = {{{{0.00f, -0.4f}}, {{-0.03f, -0.44f}}, {{-0.10f, -0.41f}}, {{-0.20f, -0.30f}}, {{-0.21f, -0.28f}}, {{-0.37f, -0.29f}}, {{-0.50f, 0.00f}}, {{-0.25f, 0.65f}}, {{0.25f, 0.65f}}, {{0.50f, 0.00f}}, {{0.39f, -0.32f}}, {{0.22f, -0.32f}}, {{0.10f, -0.41f}}, {{0.03f, -0.44f}}, {{0.00f, -0.4f}}}};

const int OUTLINE_STEPS = 24;
const int DOME_LAYERS = 256;
const float DOME_HEIGHT = 0.38f;

std::array<float, 2> catmullRom2D(const std::array<float, 2> &P0,
                                  const std::array<float, 2> &P1,
                                  const std::array<float, 2> &P2,
                                  const std::array<float, 2> &P3,
                                  float t)
{
    float t2 = t * t, t3 = t2 * t;
    std::array<float, 2> o;
    for (int i = 0; i < 2; ++i)
    {
        o[i] = 0.5f * ((2 * P1[i]) + (-P0[i] + P2[i]) * t + (2 * P0[i] - 5 * P1[i] + 4 * P2[i] - P3[i]) * t2 + (-P0[i] + 3 * P1[i] - 3 * P2[i] + P3[i]) * t3);
    }
    return o;
}

std::vector<std::array<float, 2>> makeOutline()
{
    int M = footCtrl.size() - 1;
    std::vector<std::array<float, 2>> out;
    out.reserve(M * (OUTLINE_STEPS + 1));
    for (int i = 0; i < M; ++i)
    {
        auto &P0 = footCtrl[(i - 1 + M) % M];
        auto &P1 = footCtrl[i];
        auto &P2 = footCtrl[i + 1];
        auto &P3 = footCtrl[(i + 2) % M];
        for (int s = 0; s <= OUTLINE_STEPS; ++s)
        {
            float t = float(s) / OUTLINE_STEPS;
            out.push_back(catmullRom2D(P0, P1, P2, P3, t));
        }
    }
    return out;
}

struct Vec3
{
    float x, y, z;
    Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
    void normalize()
    {
        float L = sqrtf(x * x + y * y + z * z);
        if (L > 1e-6f)
        {
            x /= L;
            y /= L;
            z /= L;
        }
    }
};

void drawFootWithToes(float xOffset)
{
    glPushMatrix();
    glTranslatef(xOffset, 0.0f, 0.0f);

    static auto outline = makeOutline();
    int L = outline.size();
    static std::vector<std::vector<Vec3>> dome;
    if (dome.empty())
    {
        dome.resize(DOME_LAYERS);
        for (int k = 0; k < DOME_LAYERS; ++k)
        {
            float phi = float(k) / (DOME_LAYERS - 1) * (M_PI / 2);
            float scale = cosf(phi);
            float y = DOME_HEIGHT * sinf(phi);
            auto &ring = dome[k];
            ring.reserve(L);
            for (auto &v : outline)
                ring.emplace_back(v[0] * scale, y, v[1] * scale);
        }
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glColor3f(1, 1, 1);

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glVertex3f(0, DOME_HEIGHT, 0);
    for (auto &P : dome[0])
        glVertex3f(P.x, P.y, P.z);
    glEnd();

    for (int k = 0; k < DOME_LAYERS - 1; ++k)
    {
        auto &A = dome[k], &B = dome[k + 1];
        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= L; ++i)
        {
            int idx = i % L;
            Vec3 normal(B[idx].x - A[idx].x, B[idx].y - A[idx].y, B[idx].z - A[idx].z);
            normal.normalize();
            glNormal3f(normal.x, normal.y, normal.z);
            glVertex3f(A[idx].x, A[idx].y, A[idx].z);
            glVertex3f(B[idx].x, B[idx].y, B[idx].z);
        }
        glEnd();
    }

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glVertex3f(0, 0, 0);
    for (int i = L - 1; i >= 0; --i)
    {
        auto &P = dome.back()[i];
        glVertex3f(P.x, P.y, P.z);
    }
    glEnd();

    struct Toe
    {
        float x, z;
    } toes[3] = {
        {-0.28f, 0.5f}, {0.0f, 0.6f}, {0.28f, 0.5f}};

    for (int i = 0; i < 3; ++i)
    {
        auto &toe = toes[i];
        glPushMatrix();
        glTranslatef(toe.x, 0.13f, -toe.z * 0.5f);
        GLdouble eqn[] = {0.0, 1.0, 0.0, 0.13};
        glClipPlane(GL_CLIP_PLANE0, eqn);
        glEnable(GL_CLIP_PLANE0);
        if (i == 1)
            glScalef(1.1f, 1.0f, 1.4f);
        else
            glScalef(1.0f, 1.0f, 1.2f);
        glutSolidSphere(0.25, 24, 24);
        glEnd();
        glDisable(GL_CLIP_PLANE0);
        glPopMatrix();
    }

    glPushMatrix();
    glTranslatef(0.0f, 0.17f, 0.3f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric *quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluCylinder(quad, 0.4, 0.4, 3.0, 256, 256);
    gluDeleteQuadric(quad);
    glPopMatrix();

    glPopMatrix();
}