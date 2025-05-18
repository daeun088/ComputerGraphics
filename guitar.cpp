// main.cpp - Textured Guitar with Proper Neck Placement and State Management
#include "guitar.h"
//#include <GL/glut.h>
#include <vector>
#include <array>
#include <cmath>
#include <iostream>
// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// guitar.cpp 상단에
extern float angleX;
extern float angleY;
extern float zoom;

GLuint texFront, texBack, texSide, texHead, texNeck;

static const std::array<std::array<float,2>,17> outline = {{
    {{  0.00f, 0.03f }}, {{  0.14f, 0.046f }}, {{  0.197f, 0.13f }},
    {{  0.195f, 0.222f }}, {{  0.194f, 0.37f }}, {{  0.267f, 0.54f}},
    {{  0.270f, 0.74f }}, {{  0.170f, 0.86f }}, {{  0.00f, 0.887f }},
    {{ -0.170f, 0.86f }}, {{ -0.270f, 0.74f }}, {{ -0.267f, 0.54f }},
    {{ -0.194f, 0.37f }}, {{ -0.195f, 0.222f }}, {{ -0.197f, 0.13f }},
    {{ -0.14f, 0.046f }}, {{  0.00f, 0.03f }}
}};

std::vector<std::array<float, 3>> pegPositions;
// 예시 텍스처 UV 위치 (추정)
std::vector<std::array<float, 2>> pegTexUVs = {
    {0.4f, 0.25f}, {0.56f, 0.25f},
    {0.2f, 0.72f}, {0.73f, 0.72f},
    {0.3f, 0.5f}, {0.65f, 0.5f},
};

std::array<float, 3> pegUVtoPosition(float u, float v) {
    const float topZ = 0.0f, bottomZ = -0.25f;
    const float topW = 0.07f, bottomW = 0.12f;
    const float y = 0.015f;  // 앞면 중심 (윗면 높이)

    float t = v;  // 이제는 그대로 v (텍스처 상에서 위쪽이 0, 아래쪽이 1)
    float z = topZ + t * (bottomZ - topZ);
    float w = topW + t * (bottomW - topW);  // 현재 깊이에서의 폭
    float x = (u - 0.5f) * w;

    return { x, y, z };
}

std::array<float,2> catmullRom(const std::array<float,2>& P0,
                               const std::array<float,2>& P1,
                               const std::array<float,2>& P2,
                               const std::array<float,2>& P3,
                               float t) {
    float t2 = t*t, t3 = t2*t;
    std::array<float,2> out;
    for(int i = 0; i < 2; ++i) {
        out[i] = 0.5f * ((2*P1[i]) + (-P0[i] + P2[i])*t +
                         (2*P0[i] - 5*P1[i] + 4*P2[i] - P3[i])*t2 +
                         (-P0[i] + 3*P1[i] - 3*P2[i] + P3[i])*t3);
    }
    return out;
}

std::vector<std::array<float,2>> makeSmoothOutline() {
    const int M = outline.size();
    const int STEPS = 8;
    std::vector<std::array<float,2>> smooth;
    for(int i = 0; i < M; ++i) {
        auto& P0 = outline[(i-1+M)%M];
        auto& P1 = outline[i];
        auto& P2 = outline[(i+1)%M];
        auto& P3 = outline[(i+2)%M];
        for(int s = 0; s <= STEPS; ++s) {
            float t = (float)s / STEPS;
            smooth.push_back(catmullRom(P0,P1,P2,P3,t));
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
        neckEndPositions.push_back({offsetX, baseY, baseZ});
    }
}


GLuint loadTexture2(const char* filename) {
    int w, h, ch;
    unsigned char* img = stbi_load(filename, &w, &h, &ch, 4);
    if (!img) { std::cerr << "Failed to load: " << filename << std::endl; return 0; }
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
    texFront = loadTexture2("guitar_body.png");
    texBack  = loadTexture2("guitar_back.png");
    texSide  = loadTexture2("guitar_side.png");
    texHead  = loadTexture2("guitar_headstock.png");
    texNeck  = loadTexture2("guitar_neck.png");
}



void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    GLfloat amb[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat dif[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat pos[] = {-0.6f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat spec[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16.0f);
}

void drawTuningKnobs(float offsetX) {
    // offsetX: 좌우로 분리된 위치 (-X는 왼쪽, +X는 오른쪽)
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    glColor3f(0.95f, 0.95f, 0.95f); // 흰색 노브

    for (int i = 0; i < 3; ++i) {
        float z = -0.1f + i * 0.05f;

        glPushMatrix();
            glTranslatef(offsetX, 0.0f, z);          // 위치 이동
            glRotatef(90, 0, 1, 0);                  // 원기둥 방향 조정
            gluCylinder(quad, 0.012f, 0.012f, 0.035f, 10, 1); // 축
            glTranslatef(0.0f, 0.0f, 0.02f);         // 축 끝에 노브
            glutSolidSphere(0.1f, 16, 16);          // 노브 구체
        glPopMatrix();
    }

    gluDeleteQuadric(quad);
}

void drawSingleHeadAngleLine(const std::array<float,3>& peg) {
    float angleDeg = 13.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = -0.16f;               // +방향 → 오른쪽으로 기울기
    float dy = -sin(angleRad);      // 아래로 기울기
    float dz = -cos(angleRad);      // z축 뒤로 기울기
    float length = 0.07f;

    float yOffset = 0.05f;
    float xOffset = 0.027f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] +  yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // 노란색
    glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
    glEnd();
}

void drawSingleHeadAngleLine2(const std::array<float,3>& peg) {
    float angleDeg = 13.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = 0.1f;               // +방향 → 오른쪽으로 기울기
    float dy = -sin(angleRad);      // 아래로 기울기
    float dz = -cos(angleRad);      // z축 뒤로 기울기
    float length = 0.07f;

    float yOffset = 0.05f;
    float xOffset = 0.03f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] +  yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // 노란색
    glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
    glEnd();
}
void drawSingleHeadAngleLine3(const std::array<float,3>& peg) {
    float angleDeg = 13.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = 0.1f;               // +방향 → 오른쪽으로 기울기
    float dy = -sin(angleRad);      // 아래로 기울기
    float dz = -cos(angleRad);      // z축 뒤로 기울기
    float length = 0.2f;

    float yOffset = 0.05f;
    float xOffset = 0.04f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] +  yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // 노란색
    glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
    glEnd();
}
void drawSingleHeadAngleLine4(const std::array<float,3>& peg) {
    float angleDeg = 12.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = 0.07f;               // +방향 → 오른쪽으로 기울기
    float dy = -sin(angleRad);      // 아래로 기울기
    float dz = -cos(angleRad);      // z축 뒤로 기울기
    float length = 0.18f;

    float yOffset = 0.05f;
    float xOffset = 0.049f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] +  yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // 노란색
    glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
    glEnd();
}

void drawSingleHeadAngleLine5(const std::array<float,3>& peg) {
     float angleDeg = 12.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = -0.13f;               // +방향 → 오른쪽으로 기울기
    float dy = -sin(angleRad);      // 아래로 기울기
    float dz = -cos(angleRad);      // z축 뒤로 기울기
    float length = 0.13f;

    float yOffset = 0.05f;
    float xOffset = 0.018f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] +  yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // 노란색
    glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
    glEnd();
}

void drawSingleHeadAngleLine6(const std::array<float,3>& peg) {
    float angleDeg = 12.0f;
    float angleRad = angleDeg * M_PI / 180.0f;

    float dx = -0.09f;               // +방향 → 오른쪽으로 기울기
    float dy = -sin(angleRad);      // 아래로 기울기
    float dz = -cos(angleRad);      // z축 뒤로 기울기
    float length = 0.18f;

    float yOffset = 0.05f;
    float xOffset = 0.01f;
    float zOffset = -0.22f;

    float x1 = peg[0] + xOffset;
    float y1 = peg[1] +  yOffset;
    float z1 = peg[2] + zOffset;


    float x2 = x1 + dx * length;
    float y2 = y1 + dy * length;
    float z2 = z1 + dz * length;

    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // 노란색
    glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
    glEnd();
}


void drawGuitarStringMarkers() {
    glColor3f(1.0f, 0.0f, 0.0f); // 빨간 점

    // 넥 기준 방향 벡터 계산
    std::array<float, 3> neckStart = { -0.025f, 0.015f, 0.18f };
    std::array<float, 3> neckEnd   = { -0.020f, 0.015f, -0.12f };
    float dx = neckEnd[0] - neckStart[0];
    float dz = neckEnd[2] - neckStart[2];
    float len = sqrt(dx*dx + dz*dz);
    float dirX = dx / len;
    float dirZ = dz / len;

    float baseX = -0.03f;  // 첫 줄 시작 x 기준
    float deltaX = 0.012f; // 줄 간격 (좁게)
    for (int i = 0; i < 6; ++i) {
        float offset = baseX + i * deltaX;

        glPushMatrix();
            glTranslatef(neckStart[0] + dirX * offset, neckStart[1], neckStart[2] + dirZ * offset);
            glutSolidSphere(0.003f, 10, 10);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(neckEnd[0] + dirX * offset, neckEnd[1], neckEnd[2] + dirZ * offset);
            glutSolidSphere(0.003f, 10, 10);
        glPopMatrix();
    }
}


void drawNeckEndpointsDebug() {
    const float baseY = 0.015f;

    // 넥 시작점 (바디와 닿는 쪽)
    std::array<float, 3> neckStart = { 0.0f, -0.4f, -0.06f };

    // 넥 끝점 (헤드 시작점)
    std::array<float, 3> neckEnd   = { 0.0f, 0.60f, -0.06f };

    glColor3f(1.0f, 0.0f, 0.0f);  // 빨간색

    glPushMatrix();
        glTranslatef(neckStart[0], neckStart[1], neckStart[2]);
        glutSolidSphere(0.005f, 12, 12);  // 시작점
    glPopMatrix();

    

    glPushMatrix();
        glTranslatef(neckEnd[0], neckEnd[1], neckEnd[2]);
        glutSolidSphere(0.005f, 12, 12);  // 끝점
    glPopMatrix();
}
void drawStringsAlongNeck() {
    glLineWidth(1.0f);
    glColor3f(0.9f, 0.9f, 0.9f);  // 밝은 회색 줄

    // 기준 줄 방향
    std::array<float, 3> baseStart = { 0.0f, -0.58f, 0.067f };  // 바디 쪽
    std::array<float, 3> baseEnd   = { 0.0f,  0.4f, 0.067f };  // 헤드 쪽

    const float neckWidth = 0.05f;   // 전체 넥 폭 (6줄 간격)
    const int stringCount = 6;

    glBegin(GL_LINES);
    for (int i = 0; i < stringCount; ++i) {
        float t = static_cast<float>(i) / (stringCount - 1);   // 0 ~ 1
        float offsetX = -neckWidth / 2 + t * neckWidth;        // 좌측에서 우측으로

        glVertex3f(baseStart[0] + offsetX, baseStart[1], baseStart[2]);
        glVertex3f(baseEnd[0] + offsetX, baseEnd[1], baseEnd[2]);
    }
    glEnd();
}



void drawTuningKnobsOnTrapezoid(bool isRight) {
    const float topZ = 0.0f, bottomZ = -0.25f;
    const float topW = 0.07f, bottomW = 0.12f;

    // Z 방향으로 3개의 위치 정의 (헤드 위쪽부터 아래까지)
    std::vector<float> zPos = { -0.06f, -0.12f, -0.18f }; 
    if (!isRight) std::reverse(zPos.begin(), zPos.end());  // 왼쪽일 땐 z 순서를 반대로

    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    glColor3f(0.8f, 0.8f, 0.8f); // 중간 회색: 튜닝 노브에 어울리는 무광 느낌 // 하얀색 노브

    for (auto z : zPos) {
        float t = (z - topZ) / (bottomZ - topZ);         // 깊이에 따른 상대 위치
        float w = topW + (bottomW - topW) * t;           // 현재 깊이의 헤드 폭
        float offsetX = (w / 2.0f + 0.005f) * (isRight ? 1.0f : -1.0f); // 좌우 대칭 X 위치
        

        float pegOffset = 0.01f; // 줄이 헤드 안쪽에서 시작하도록 약간 밀기
float pegX = offsetX - (isRight ? pegOffset : -pegOffset); // 안쪽으로
// pegPositions.push_back({pegX, 0.0f, z});

        glPushMatrix();
            glTranslatef(offsetX, 0.0f, z);               // 위치 이동
            glRotatef(90, 0, 1, 0);                       // 원기둥이 Z축 방향이 되도록 회전
            gluCylinder(quad, 0.004f, 0.004f, 0.02f, 10, 1);  // 축
            glTranslatef(0, 0, 0.02f);                    // 축 끝에 노브
            glutSolidSphere(0.01f, 12, 12);               // 노브 구체
        glPopMatrix();
    }
    

    gluDeleteQuadric(quad);
}

void drawLeftTuningKnobsOnTrapezoid() {
    const float topZ = 0.0f, bottomZ = -0.25f;
    const float topW = 0.07f, bottomW = 0.12f;

    std::vector<float> zPos = { -0.06f, -0.12f, -0.18f };  // 살짝 다르게 배치
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);
    glColor3f(0.8f, 0.8f, 0.8f); // 중간 회색: 튜닝 노브에 어울리는 무광 느낌 // 하얀색 노브

    for (auto z : zPos) {
        float t = (z - topZ) / (bottomZ - topZ);
        float w = topW + (bottomW - topW) * t;
        float offsetX = -(w / 2.0f + 0.005f);  // 왼쪽으로 반전
        //pegPositions.push_back({offsetX, 0.0f, z});

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
void connectNeckEndsToPegs(const std::vector<std::array<float, 3>>& pegs) {
    const float neckWidth = 0.05f;
    const int stringCount = 6;

    std::array<float, 3> baseEnd = { 0.0f, -0.40f, -0.07f }; // 넥 끝점 (헤드쪽)

    glLineWidth(1.0f);
    glColor3f(0.85f, 0.85f, 0.85f);  // 줄 색

    glBegin(GL_LINES);
    for (int i = 0; i < stringCount; ++i) {
        float t = static_cast<float>(i) / (stringCount - 1);
        float offsetX = -neckWidth / 2 + t * neckWidth;

        float x = baseEnd[0] + offsetX;
        float y = baseEnd[1];
        float z = baseEnd[2];

        const auto& peg = pegs[i];
        glVertex3f(x, y, z);        // 넥 쪽 줄 시작점
        glVertex3f(peg[0], peg[1], peg[2]);  // 페그 줄 끝점
    }
    glEnd();
}

void drawNeckHead() {
    pegPositions.clear();
    glPushMatrix();
    glTranslatef(0.0f, 0.04f, -0.4f);
    glScalef(1.0f, 1.4f, 1.0f);   // 넥의 끝 위치
    glRotatef(8.0f, -1.0f, 0.0f, 0.0f);  // +X축 회전 → 헤드를 +Y 방향으로 살짝 기울임
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texHead);
    glColor3f(1.0f, 1.0f, 1.0f);  

    const float topZ = 0.0f, bottomZ = -0.25f;
    const float topW = 0.07f, bottomW = 0.12f;  // 윗면이 좁고 아래가 넓은 사다리꼴

    const float h = 0.03f;
    

    // 윗면 (텍스처 매핑 대상)
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-topW/2, +h/2, topZ);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(+topW/2, +h/2, topZ);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(+bottomW/2, +h/2, bottomZ);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-bottomW/2, +h/2, bottomZ);
    glEnd();

    // 나머지 면 → guitar_side 텍스처
    glBindTexture(GL_TEXTURE_2D, texSide);

    // 아래면
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-topW/2, -h/2, topZ);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(+topW/2, -h/2, topZ);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(+bottomW/2, -h/2, bottomZ);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-bottomW/2, -h/2, bottomZ);
    glEnd();

    // 왼쪽
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-topW/2, -h/2, topZ);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-topW/2, +h/2, topZ);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-bottomW/2, +h/2, bottomZ);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-bottomW/2, -h/2, bottomZ);
    glEnd();

    // 오른쪽
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(+topW/2, -h/2, topZ);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(+topW/2, +h/2, topZ);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(+bottomW/2, +h/2, bottomZ);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(+bottomW/2, -h/2, bottomZ);
    glEnd();

    // 뒷면
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-bottomW/2, -h/2, bottomZ);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(+bottomW/2, -h/2, bottomZ);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(+bottomW/2, +h/2, bottomZ);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-bottomW/2, +h/2, bottomZ);
    glEnd();

    glDisable(GL_TEXTURE_2D);


     // ✅ 튜닝 노브 추가 위치 (헤드 기준 좌우)
    //  drawTuningKnobs(+0.06f);  // 오른쪽
    //  drawTuningKnobs(-0.06f); 

    // 튜닝 노브 배치
    pegPositions.clear();
    for (auto& uv : pegTexUVs) {
        pegPositions.push_back(pegUVtoPosition(uv[0], uv[1]));
    
        // 디버깅용 점
        
    }
    std::sort(pegPositions.begin(), pegPositions.end(), [](const auto& a, const auto& b) {
    return a[0] > b[0];  // x축 기준 정렬
});

    //connectNeckEndsToPegs(pegPositions);
     
    drawTuningKnobsOnTrapezoid(true);
    drawLeftTuningKnobsOnTrapezoid();
    //drawGuitarStrings(pegPositions);
    glPopMatrix();
    // pegPositions를 x 기준 오름차순 정렬
std::sort(pegPositions.begin(), pegPositions.end(), [](const auto& a, const auto& b) {
    return a[0] < b[0];
});
    
    //drawStringsFromNeckToPegs(pegPositions);



  

    //
}



void drawNeck() {
    const float w = 0.08f, h = 0.03f, l = 0.30f;

    glPushMatrix();
    glTranslatef(0.0f, 0.04f, 0.16f);
    glScalef(1.0f, 1.4f, 1.9f);  // 바디에 살짝 겹치도록 위치 조정
    //glScalef(0.08f, 0.05f, 0.6f);
    // 👉 앞면 텍스처: 윗 d면(y = +h/2)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texNeck);
    glColor3f(1.0f, 1.0f, 1.0f);  
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-w/2, h/2, 0);
        glTexCoord2f(1, 0); glVertex3f( w/2, h/2, 0);
        glTexCoord2f(1, 1); glVertex3f( w/2, h/2, -l);
        glTexCoord2f(0, 1); glVertex3f(-w/2, h/2, -l);
    glEnd();


    // 👉 나머지 면은 단색
      // 나머지 면 텍스처 (guitar_side)
      glBindTexture(GL_TEXTURE_2D, texSide);
      glBegin(GL_QUADS);
          // 뒷면
          glTexCoord2f(0, 0); glVertex3f(-w/2, -h/2, 0);
          glTexCoord2f(1, 0); glVertex3f(-w/2, -h/2, -l);
          glTexCoord2f(1, 1); glVertex3f( w/2, -h/2, -l);
          glTexCoord2f(0, 1); glVertex3f( w/2, -h/2, 0);
  
          // 왼쪽
          glTexCoord2f(0, 0); glVertex3f(-w/2, -h/2, 0);
          glTexCoord2f(1, 0); glVertex3f(-w/2,  h/2, 0);
          glTexCoord2f(1, 1); glVertex3f(-w/2,  h/2, -l);
          glTexCoord2f(0, 1); glVertex3f(-w/2, -h/2, -l);
  
          // 오른쪽
          glTexCoord2f(0, 0); glVertex3f(w/2, -h/2, 0);
          glTexCoord2f(1, 0); glVertex3f(w/2,  h/2, 0);
          glTexCoord2f(1, 1); glVertex3f(w/2,  h/2, -l);
          glTexCoord2f(0, 1); glVertex3f(w/2, -h/2, -l);
  
          // 바닥
          glTexCoord2f(0, 0); glVertex3f(-w/2, -h/2, 0);
          glTexCoord2f(1, 0); glVertex3f(-w/2, -h/2, -l);
          glTexCoord2f(1, 1); glVertex3f( w/2, -h/2, -l);
          glTexCoord2f(0, 1); glVertex3f( w/2, -h/2, 0);
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

    // Side
    glBindTexture(GL_TEXTURE_2D, texSide);
    glColor3f(1.0f, 1.0f, 1.0f);  
    glBegin(GL_QUAD_STRIP);
    for (size_t i = 0; i < smooth.size(); ++i) {
        float u = float(i) / smooth.size();
        glTexCoord2f(u, 0); glVertex3f(smooth[i][0], +halfThick, smooth[i][1]);
        glTexCoord2f(u, 1); glVertex3f(smooth[i][0], -halfThick, smooth[i][1]);
    }
    glEnd();

    // Front
    glBindTexture(GL_TEXTURE_2D, texFront);
    glColor3f(1.0f, 1.0f, 1.0f);  
    glBegin(GL_POLYGON);
    for (auto& v : smooth) {
        float u = (v[0] - minX)/(maxX-minX), vTex = (v[1] - minZ)/(maxZ-minZ);
        glTexCoord2f(vTex, 1.0f - u);
        glVertex3f(v[0], +halfThick, v[1]);
    }
    glEnd();

    // Back
    glBindTexture(GL_TEXTURE_2D, texBack);
    glColor3f(1.0f, 1.0f, 1.0f);  
    glBegin(GL_POLYGON);
    for (auto it = smooth.rbegin(); it != smooth.rend(); ++it) {
        float u = ((*it)[0] - minX)/(maxX-minX), vTex = ((*it)[1] - minZ)/(maxZ-minZ);
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
    drawSingleHeadAngleLine4(pegPositions[0]);// 첫 번째 페그에서 연장선 그리기
    drawSingleHeadAngleLine5(pegPositions[0]);
    drawSingleHeadAngleLine6(pegPositions[0]);
}

    glPopMatrix();

    //drawNeckStringsDebug();
    //drawGuitarStringMarkers();
    //drawGuitarStringsFromNeckEnd();
   // drawStringBridgesToPegs();

    //drawNeckEndpointsDebug();

    


    drawStringsAlongNeck();
   // connectNeckEndsToPegs();
   //drawNeckEndDebugDots();

   generateNeckEndPositions(); 
    // 꼭 먼저 호출!
//drawStringsBetweenPegAndNeck();
//debugDrawSingleString();          // ← 여기에 한 줄 디버깅 추가!






}

