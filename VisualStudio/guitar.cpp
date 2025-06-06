#include "kk_headers.h"

#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

#include "stb_image.h"


extern float angleX;
extern float angleY;
extern float zoom;

GLuint texFront, texBack, texSide, texHead, texNeck;

static const std::array<std::array<float, 2>, 17> outline = { {
   {{  0.00f, 0.03f }}, {{  0.14f, 0.046f }}, {{  0.197f, 0.13f }},
   {{  0.195f, 0.222f }}, {{  0.194f, 0.37f }}, {{  0.267f, 0.54f}},
   {{  0.270f, 0.74f }}, {{  0.170f, 0.86f }}, {{  0.00f, 0.887f }},
   {{ -0.170f, 0.86f }}, {{ -0.270f, 0.74f }}, {{ -0.267f, 0.54f }},
   {{ -0.194f, 0.37f }}, {{ -0.195f, 0.222f }}, {{ -0.197f, 0.13f }},
   {{ -0.14f, 0.046f }}, {{  0.00f, 0.03f }}
} };

std::vector<std::array<float, 3>> pegPositions;
std::vector<std::array<float, 2>> pegTexUVs = {
   {0.4f, 0.25f}, {0.56f, 0.25f},
   {0.2f, 0.72f}, {0.73f, 0.72f},
   {0.3f, 0.5f}, {0.65f, 0.5f},
};

std::array<float, 3> pegUVtoPosition(float u, float v) {
    const float topZ = 0.0f, bottomZ = -0.25f;
    const float topW = 0.07f, bottomW = 0.12f;
    const float y = 0.015f;

    float t = v;
    float z = topZ + t * (bottomZ - topZ);
    float w = topW + t * (bottomW - topW);
    float x = (u - 0.5f) * w;

    return { x, y, z };
}

std::array<float, 2> catmullRom(const std::array<float, 2>& P0,
    const std::array<float, 2>& P1,
    const std::array<float, 2>& P2,
    const std::array<float, 2>& P3,
    float t) {
    float t2 = t * t, t3 = t2 * t;
    std::array<float, 2> out;
    for (int i = 0; i < 2; ++i) {
        out[i] = 0.5f * ((2 * P1[i]) + (-P0[i] + P2[i]) * t +
            (2 * P0[i] - 5 * P1[i] + 4 * P2[i] - P3[i]) * t2 +
            (-P0[i] + 3 * P1[i] - 3 * P2[i] + P3[i]) * t3);
    }
    return out;
}

std::vector<std::array<float, 2>> makeSmoothOutline() {
    const int M = outline.size();
    const int STEPS = 8;
    std::vector<std::array<float, 2>> smooth;
    for (int i = 0; i < M; ++i) {
        auto& P0 = outline[(i - 1 + M) % M];
        auto& P1 = outline[i];
        auto& P2 = outline[(i + 1) % M];
        auto& P3 = outline[(i + 2) % M];
        for (int s = 0; s <= STEPS; ++s) {
            float t = (float)s / STEPS;
            smooth.push_back(catmullRom(P0, P1, P2, P3, t));
        }
    }
    return smooth;
}

std::vector<std::array<float, 3>> neckEndPositions;
void generateNeckEndPositions() {
    neckEndPositions.clear();
    const float neckWidth = 0.057f;
    const float baseY = -0.40f;
    const float baseZ = -0.07f;

    for (int i = 0; i < 6; ++i) {
        float t = static_cast<float>(i) / 5.0f;
        float offsetX = -neckWidth / 2 + t * neckWidth;
        neckEndPositions.push_back({ offsetX, baseY, baseZ });
    }
}


GLuint loadTexture2(const char* filename) {
    int w, h, ch;
    unsigned char* img = stbi_load(filename, &w, &h, &ch, 4);
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(img);
    return texID;
}



void loadAllTextures() {
    texFront = loadTexture2("Image/guitar/guitar_body.png");
    texBack = loadTexture2("Image/guitar/guitar_back.png");
    texSide = loadTexture2("Image/guitar/guitar_side.png");
    texHead = loadTexture2("Image/guitar/guitar_headstock.png");
    texNeck = loadTexture2("Image/guitar/guitar_neck.png");
}



void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    GLfloat amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat dif[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat pos[] = { -0.6f, 1.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat spec[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16.0f);
}

void drawTuningKnobs(float offsetX) {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    glColor3f(0.95f, 0.95f, 0.95f);

    for (int i = 0; i < 3; ++i) {
        float z = -0.1f + i * 0.05f;

        glPushMatrix();
        glTranslatef(offsetX, 0.0f, z);
        glRotatef(90, 0, 1, 0);
        gluCylinder(quad, 0.012f, 0.012f, 0.035f, 10, 1);
        glTranslatef(0.0f, 0.0f, 0.02f);
        glutSolidSphere(0.1f, 16, 16);
        glPopMatrix();
    }

    gluDeleteQuadric(quad);
}

void drawSingleHeadAngleLine(const std::array<float, 3>& peg) {
    float angleDeg = 13.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = -0.16f;
    float dy = -sin(angleRad);
    float dz = -cos(angleRad);
    float length = 0.07f;

    float yOffset = 0.05f;
    float xOffset = 0.027f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] + yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}

void drawSingleHeadAngleLine2(const std::array<float, 3>& peg) {
    float angleDeg = 13.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = 0.1f;
    float dy = -sin(angleRad);
    float dz = -cos(angleRad);
    float length = 0.07f;

    float yOffset = 0.05f;
    float xOffset = 0.03f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] + yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}
void drawSingleHeadAngleLine3(const std::array<float, 3>& peg) {
    float angleDeg = 13.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = 0.1f;
    float dy = -sin(angleRad);
    float dz = -cos(angleRad);
    float length = 0.2f;

    float yOffset = 0.05f;
    float xOffset = 0.04f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] + yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}

void drawSingleHeadAngleLine4(const std::array<float, 3>& peg) {
    float angleDeg = 12.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = 0.07f;
    float dy = -sin(angleRad);
    float dz = -cos(angleRad);
    float length = 0.18f;

    float yOffset = 0.05f;
    float xOffset = 0.049f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] + yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}

void drawSingleHeadAngleLine5(const std::array<float, 3>& peg) {
    float angleDeg = 12.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = -0.13f;
    float dy = -sin(angleRad);
    float dz = -cos(angleRad);
    float length = 0.13f;

    float yOffset = 0.05f;
    float xOffset = 0.018f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] + yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}

void drawSingleHeadAngleLine6(const std::array<float, 3>& peg) {
    float angleDeg = 12.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = -0.09f;
    float dy = -sin(angleRad);
    float dz = -cos(angleRad);
    float length = 0.18f;

    float yOffset = 0.05f;
    float xOffset = 0.01f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] + yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}


void drawStringsAlongNeck() {
    glLineWidth(1.0f);
    glColor3f(0.9f, 0.9f, 0.9f);

    std::array<float, 3> baseStart = { 0.0f, -0.58f, 0.067f };
    std::array<float, 3> baseEnd = { 0.0f,  0.4f, 0.067f };

    const float neckWidth = 0.05f;
    const int stringCount = 6;

    glBegin(GL_LINES);
    for (int i = 0; i < stringCount; ++i) {
        float t = static_cast<float>(i) / (stringCount - 1);
        float offsetX = -neckWidth / 2 + t * neckWidth;

        glVertex3f(baseStart[0] + offsetX, baseStart[1], baseStart[2]);
        glVertex3f(baseEnd[0] + offsetX, baseEnd[1], baseEnd[2]);
    }
    glEnd();
}



void drawTuningKnobsOnTrapezoid(bool isRight) {
    const float topZ = 0.0f, bottomZ = -0.25f;
    const float topW = 0.07f, bottomW = 0.12f;


    std::vector<float> zPos = { -0.06f, -0.12f, -0.18f };
    if (!isRight) std::reverse(zPos.begin(), zPos.end());

    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    glColor3f(0.8f, 0.8f, 0.8f);

    for (auto z : zPos) {
        float t = (z - topZ) / (bottomZ - topZ);
        float w = topW + (bottomW - topW) * t;
        float offsetX = (w / 2.0f + 0.005f) * (isRight ? 1.0f : -1.0f);


        float pegOffset = 0.01f;
        float pegX = offsetX - (isRight ? pegOffset : -pegOffset);


        glPushMatrix();
        glTranslatef(offsetX, 0.0f, z);
        glRotatef(90, 0, 1, 0);
        gluCylinder(quad, 0.004f, 0.004f, 0.02f, 10, 1);
        glTranslatef(0, 0, 0.02f);
        glutSolidSphere(0.01f, 12, 12);
        glPopMatrix();
    }


    gluDeleteQuadric(quad);
}

void drawLeftTuningKnobsOnTrapezoid() {
    const float topZ = 0.0f, bottomZ = -0.25f;
    const float topW = 0.07f, bottomW = 0.12f;

    std::vector<float> zPos = { -0.06f, -0.12f, -0.18f };
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    glColor3f(0.8f, 0.8f, 0.8f);

    for (auto z : zPos) {
        float t = (z - topZ) / (bottomZ - topZ);
        float w = topW + (bottomW - topW) * t;
        float offsetX = -(w / 2.0f + 0.005f);


        glPushMatrix();
        glTranslatef(offsetX, 0.0f, z);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(quad, 0.004f, 0.004f, 0.02f, 10, 1);
        glTranslatef(0, 0, 0.02f);
        glutSolidSphere(0.01f, 12, 12);
        glPopMatrix();
    }

    gluDeleteQuadric(quad);
}

void drawNeckHead() {
    pegPositions.clear();
    glPushMatrix();
    glTranslatef(0.0f, 0.04f, -0.4f);
    glScalef(1.0f, 1.4f, 1.0f);
    glRotatef(8.0f, -1.0f, 0.0f, 0.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texHead);
    glColor3f(1.0f, 1.0f, 1.0f);

    const float topZ = 0.0f, bottomZ = -0.25f;
    const float topW = 0.07f, bottomW = 0.12f;

    const float h = 0.03f;



    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-topW / 2, +h / 2, topZ);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(+topW / 2, +h / 2, topZ);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(+bottomW / 2, +h / 2, bottomZ);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-bottomW / 2, +h / 2, bottomZ);
    glEnd();


    glBindTexture(GL_TEXTURE_2D, texSide);


    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-topW / 2, -h / 2, topZ);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(+topW / 2, -h / 2, topZ);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(+bottomW / 2, -h / 2, bottomZ);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-bottomW / 2, -h / 2, bottomZ);
    glEnd();


    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-topW / 2, -h / 2, topZ);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-topW / 2, +h / 2, topZ);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-bottomW / 2, +h / 2, bottomZ);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-bottomW / 2, -h / 2, bottomZ);
    glEnd();


    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(+topW / 2, -h / 2, topZ);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(+topW / 2, +h / 2, topZ);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(+bottomW / 2, +h / 2, bottomZ);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(+bottomW / 2, -h / 2, bottomZ);
    glEnd();


    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-bottomW / 2, -h / 2, bottomZ);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(+bottomW / 2, -h / 2, bottomZ);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(+bottomW / 2, +h / 2, bottomZ);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-bottomW / 2, +h / 2, bottomZ);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    pegPositions.clear();
    for (auto& uv : pegTexUVs) {
        pegPositions.push_back(pegUVtoPosition(uv[0], uv[1]));



    }
    std::sort(pegPositions.begin(), pegPositions.end(), [](const auto& a, const auto& b) {
        return a[0] > b[0];
        });



    drawTuningKnobsOnTrapezoid(true);
    drawLeftTuningKnobsOnTrapezoid();

    glPopMatrix();

    std::sort(pegPositions.begin(), pegPositions.end(), [](const auto& a, const auto& b) {
        return a[0] < b[0];
        });




}



void drawNeck() {
    const float w = 0.08f, h = 0.03f, l = 0.30f;

    glPushMatrix();
    glTranslatef(0.0f, 0.04f, 0.16f);
    glScalef(1.0f, 1.4f, 1.9f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texNeck);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-w / 2, h / 2, 0);
    glTexCoord2f(1, 0); glVertex3f(w / 2, h / 2, 0);
    glTexCoord2f(1, 1); glVertex3f(w / 2, h / 2, -l);
    glTexCoord2f(0, 1); glVertex3f(-w / 2, h / 2, -l);
    glEnd();


    glBindTexture(GL_TEXTURE_2D, texSide);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0); glVertex3f(-w / 2, -h / 2, 0);
    glTexCoord2f(1, 0); glVertex3f(-w / 2, -h / 2, -l);
    glTexCoord2f(1, 1); glVertex3f(w / 2, -h / 2, -l);
    glTexCoord2f(0, 1); glVertex3f(w / 2, -h / 2, 0);


    glTexCoord2f(0, 0); glVertex3f(-w / 2, -h / 2, 0);
    glTexCoord2f(1, 0); glVertex3f(-w / 2, h / 2, 0);
    glTexCoord2f(1, 1); glVertex3f(-w / 2, h / 2, -l);
    glTexCoord2f(0, 1); glVertex3f(-w / 2, -h / 2, -l);


    glTexCoord2f(0, 0); glVertex3f(w / 2, -h / 2, 0);
    glTexCoord2f(1, 0); glVertex3f(w / 2, h / 2, 0);
    glTexCoord2f(1, 1); glVertex3f(w / 2, h / 2, -l);
    glTexCoord2f(0, 1); glVertex3f(w / 2, -h / 2, -l);


    glTexCoord2f(0, 0); glVertex3f(-w / 2, -h / 2, 0);
    glTexCoord2f(1, 0); glVertex3f(-w / 2, -h / 2, -l);
    glTexCoord2f(1, 1); glVertex3f(w / 2, -h / 2, -l);
    glTexCoord2f(0, 1); glVertex3f(w / 2, -h / 2, 0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}



void drawGuitar() {
    static auto smooth = makeSmoothOutline();
    const float halfThick = 0.06f;
    float minX = -0.28f, maxX = 0.28f, minZ = 0.03f, maxZ = 0.89f;

    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texSide);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUAD_STRIP);
    for (size_t i = 0; i < smooth.size(); ++i) {
        float u = float(i) / smooth.size();
        glTexCoord2f(u, 0); glVertex3f(smooth[i][0], +halfThick, smooth[i][1]);
        glTexCoord2f(u, 1); glVertex3f(smooth[i][0], -halfThick, smooth[i][1]);
    }
    glEnd();


    glBindTexture(GL_TEXTURE_2D, texFront);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    for (auto& v : smooth) {
        float u = (v[0] - minX) / (maxX - minX), vTex = (v[1] - minZ) / (maxZ - minZ);
        glTexCoord2f(vTex, 1.0f - u);
        glVertex3f(v[0], +halfThick, v[1]);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texBack);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    for (auto it = smooth.rbegin(); it != smooth.rend(); ++it) {
        float u = ((*it)[0] - minX) / (maxX - minX), vTex = ((*it)[1] - minZ) / (maxZ - minZ);
        glTexCoord2f(vTex, 1.0f - u);
        glVertex3f((*it)[0], -halfThick, (*it)[1]);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    drawNeck();
    drawNeckHead();
    if (!pegPositions.empty()) {
        drawSingleHeadAngleLine(pegPositions[0]);
        drawSingleHeadAngleLine2(pegPositions[0]);
        drawSingleHeadAngleLine3(pegPositions[0]);
        drawSingleHeadAngleLine4(pegPositions[0]);
        drawSingleHeadAngleLine5(pegPositions[0]);
        drawSingleHeadAngleLine6(pegPositions[0]);
    }

    glPopMatrix();


    drawStringsAlongNeck();

    generateNeckEndPositions();

}