// main.cpp - Textured Guitar with Proper Neck Placement and State Management
#include <GL/glut.h>
#include <vector>
#include <array>
#include <cmath>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float angleX = 20, angleY = -30, zoom = -2.5f;
GLuint texFront, texBack, texSide, texHead, texNeck;

static const std::array<std::array<float,2>,17> outline = {{
    {{  0.00f, 0.03f }}, {{  0.12f, 0.05f }}, {{  0.198f, 0.13f }},
    {{  0.205f, 0.222f }}, {{  0.198f, 0.38f }}, {{  0.275f, 0.54f}},
    {{  0.282f, 0.74f }}, {{  0.1650f, 0.87f }}, {{  0.00f, 0.89f }},
    {{ -0.1650f, 0.87f }}, {{ -0.282f, 0.74f }}, {{ -0.275f, 0.54f }},
    {{ -0.198f, 0.38f }}, {{ -0.205f, 0.222f }}, {{ -0.198f, 0.13f }},
    {{ -0.12f, 0.05f }}, {{  0.00f, 0.03f }}
}};

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

GLuint loadTexture(const char* filename) {
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




void drawTuningKnobs(float offsetX, float offsetZ) {
    glColor3f(0.95f, 0.95f, 0.95f); // 하얀색
    for (int i = 0; i < 3; ++i) {
        float z = offsetZ + i * 0.035f;
        glPushMatrix();
            glTranslatef(offsetX, 0.0f, z);
            glutSolidSphere(0.01f, 12, 12);
        glPopMatrix();
    }
}

void drawNeckHead() {
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

    glDisable(GL_TEXTURE_2D);
    glColor3f(0.35f, 0.20f, 0.10f);  // 나머지는 갈색

    // 아래면
    glBegin(GL_QUADS);
      glVertex3f(-topW/2, -h/2, topZ);
      glVertex3f(+topW/2, -h/2, topZ);
      glVertex3f(+bottomW/2, -h/2, bottomZ);
      glVertex3f(-bottomW/2, -h/2, bottomZ);
    glEnd();

    // 옆면 (좌, 우, 앞, 뒤)
    glBegin(GL_QUAD_STRIP);
      // 왼쪽
      glVertex3f(-topW/2, -h/2, topZ);
      glVertex3f(-topW/2, +h/2, topZ);
      glVertex3f(-bottomW/2, -h/2, bottomZ);
      glVertex3f(-bottomW/2, +h/2, bottomZ);

      // 오른쪽
      glVertex3f(+bottomW/2, -h/2, bottomZ);
      glVertex3f(+bottomW/2, +h/2, bottomZ);
      glVertex3f(+topW/2, -h/2, topZ);
      glVertex3f(+topW/2, +h/2, topZ);
    glEnd();

    // 앞면
    glBegin(GL_QUADS);
      glVertex3f(-topW/2, -h/2, topZ);
      glVertex3f(+topW/2, -h/2, topZ);
      glVertex3f(+topW/2, +h/2, topZ);
      glVertex3f(-topW/2, +h/2, topZ);
    glEnd();

    // 뒷면
    glBegin(GL_QUADS);
      glVertex3f(-bottomW/2, -h/2, bottomZ);
      glVertex3f(+bottomW/2, -h/2, bottomZ);
      glVertex3f(+bottomW/2, +h/2, bottomZ);
      glVertex3f(-bottomW/2, +h/2, bottomZ);
    glEnd();

    glPopMatrix();
}

// void drawNeck() {
//     glPushAttrib(GL_ALL_ATTRIB_BITS);
//     glDisable(GL_TEXTURE_2D);
//     glColor3f(0.4f, 0.2f, 0.1f); // 진한 목재색

//     glPushMatrix();
//     glTranslatef(0.0f, 0.04f, -0.13f);
//      // 기타 바디 위로 겹쳐지도록 약간 이동
//     glScalef(0.08f, 0.05f, 0.6f);    // 폭, 두께, 길이 (약간 줄임)
//     glutSolidCube(1.0);
//     glPopMatrix();

//     glPopAttrib();
// }


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
    glDisable(GL_TEXTURE_2D);

    // 👉 나머지 면은 단색
    glColor3f(0.4f, 0.2f, 0.1f);
    glBegin(GL_QUADS);
        // 뒷면(y=-h/2)
        glVertex3f(-w/2, -h/2, 0); glVertex3f(-w/2, -h/2, -l);
        glVertex3f( w/2, -h/2, -l); glVertex3f( w/2, -h/2, 0);

        // 왼쪽
        glVertex3f(-w/2, -h/2, 0); glVertex3f(-w/2,  h/2, 0);
        glVertex3f(-w/2,  h/2, -l); glVertex3f(-w/2, -h/2, -l);

        // 오른쪽
        glVertex3f(w/2, -h/2, 0); glVertex3f(w/2,  h/2, 0);
        glVertex3f(w/2,  h/2, -l); glVertex3f(w/2, -h/2, -l);

        // 바닥(y = -h/2 → 아랫면)
        glVertex3f(-w/2, -h/2, 0); glVertex3f(-w/2, -h/2, -l);
        glVertex3f( w/2, -h/2, -l); glVertex3f( w/2, -h/2, 0);
    glEnd();

    glPopMatrix();
}

void drawGuitar() {
    static auto smooth = makeSmoothOutline();
    const float halfThick = 0.05f;
    float minX = -0.28f, maxX = 0.28f, minZ = 0.03f, maxZ = 0.89f;

    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
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

    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0, 0, zoom);
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);
    drawGuitar();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 1, 100);
    glMatrixMode(GL_MODELVIEW);
}

void mouseMotion(int x, int y) {
    static int lastX = -1, lastY = -1;
    if (lastX >= 0) {
        angleY += (x - lastX) * 0.5f;
        angleX += (y - lastY) * 0.5f;
    }
    lastX = x; lastY = y;
    glutPostRedisplay();
}

void mouseWheel(int button, int state, int x, int y) {
    if (button == 3) zoom += 0.2f;
    else if (button == 4) zoom -= 0.2f;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == '+' || key == '=') zoom += 0.2f;
    else if (key == '-' || key == '_') zoom -= 0.2f;
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP) zoom += 0.2f;
    else if (key == GLUT_KEY_DOWN) zoom -= 0.2f;
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Textured Guitar with Neck");
    glClearColor(0.5f, 0.0f, 0.13f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    texFront = loadTexture("guitar_body.png");
    texBack  = loadTexture("guitar_back.png");
    texSide  = loadTexture("guitar_side.png");
    texHead = loadTexture("guitar_headstock.png");
    texNeck = loadTexture("guitar_neck.png");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseWheel);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    return 0;
}
