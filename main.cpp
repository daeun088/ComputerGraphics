// main.cpp - 양발/양다리 구조 구현 (10배 확대 버전)
#include <GL/glut.h>







#include "upper_body.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>
#include <array>
#include <cmath>



// 전역 변수 선언
int windowWidth, windowHeight;
int th, ph;
double dim;
int fov;
double asp;
double ecX, ecY, ecZ;

// 회전/줌 상태
float angleX = 0, angleY = 0, zoom = -5.0; //zoom이 처음 시작 시점 위치
int prevX, prevY;
bool isDragging = false;
GLuint eyeTexture;








static const std::array<std::array<float,2>,20> footCtrl = {{
  {{  0.00f, -0.4f }}, {{ -0.03f, -0.44f }}, {{ -0.10f, -0.41f }},
  {{ -0.20f, -0.30f }}, {{ -0.21f, -0.28f }}, {{ -0.37f, -0.29f }},
  {{ -0.50f,  0.00f }}, {{ -0.25f, 0.65f }}, {{ 0.25f, 0.65f }},
  {{ 0.50f,  0.00f }}, {{ 0.39f, -0.32f }}, {{ 0.22f, -0.32f }},
  {{ 0.10f, -0.41f }}, {{ 0.03f, -0.44f }}, {{ 0.00f, -0.4f }}
}};

const int OUTLINE_STEPS = 24;
const int DOME_LAYERS   = 256;
const float DOME_HEIGHT = 0.38f;

std::array<float,2> catmullRom2D(const std::array<float,2>& P0,
                                  const std::array<float,2>& P1,
                                  const std::array<float,2>& P2,
                                  const std::array<float,2>& P3,
                                  float t) {
  float t2 = t*t, t3 = t2*t;
  std::array<float,2> o;
  for(int i=0;i<2;++i) {
    o[i] = 0.5f*((2*P1[i])+(-P0[i]+P2[i])*t+(2*P0[i]-5*P1[i]+4*P2[i]-P3[i])*t2+(-P0[i]+3*P1[i]-3*P2[i]+P3[i])*t3);
  }
  return o;
}

std::vector<std::array<float,2>> makeOutline() {
  int M = footCtrl.size() - 1;
  std::vector<std::array<float,2>> out;
  out.reserve(M * (OUTLINE_STEPS + 1));
  for(int i=0; i<M; ++i) {
    auto& P0 = footCtrl[(i-1+M)%M];
    auto& P1 = footCtrl[i];
    auto& P2 = footCtrl[i+1];
    auto& P3 = footCtrl[(i+2)%M];
    for(int s=0; s<=OUTLINE_STEPS; ++s) {
      float t = float(s) / OUTLINE_STEPS;
      out.push_back(catmullRom2D(P0,P1,P2,P3,t));
    }
  }
  return out;
}

struct Vec3 {
  float x, y, z;
  Vec3(float X, float Y, float Z): x(X), y(Y), z(Z) {}
  void normalize() {
    float L = sqrtf(x*x + y*y + z*z);
    if (L > 1e-6f) { x /= L; y /= L; z /= L; }
  }
};

void drawFootWithToes(float xOffset) {
  glPushMatrix();
  glTranslatef(xOffset, 0.0f, 0.0f);

  static auto outline = makeOutline();
  int L = outline.size();
  static std::vector<std::vector<Vec3>> dome;
  if(dome.empty()) {
    dome.resize(DOME_LAYERS);
    for(int k=0;k<DOME_LAYERS;++k){
      float phi = float(k)/(DOME_LAYERS-1) * (M_PI/2);
      float scale = cosf(phi);
      float y     = DOME_HEIGHT * sinf(phi);
      auto& ring  = dome[k];
      ring.reserve(L);
      for(auto& v: outline)
        ring.emplace_back(v[0]*scale, y, v[1]*scale);
    }
  }

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);
    glVertex3f(0, DOME_HEIGHT, 0);
    for(auto& P: dome[0]) glVertex3f(P.x, P.y, P.z);
  glEnd();

  for(int k=0;k<DOME_LAYERS-1;++k){
    auto &A = dome[k], &B = dome[k+1];
    glBegin(GL_TRIANGLE_STRIP);
      for(int i=0;i<=L;++i){
        int idx = i%L;
        Vec3 normal(B[idx].x - A[idx].x, B[idx].y - A[idx].y, B[idx].z - A[idx].z);
        normal.normalize();
        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(A[idx].x, A[idx].y, A[idx].z);
        glVertex3f(B[idx].x, B[idx].y, B[idx].z);
      }
    glEnd();
  }

  glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,-1,0);
    glVertex3f(0, 0, 0);
    for(int i=L-1;i>=0;--i){
      auto& P = dome.back()[i];
      glVertex3f(P.x, P.y, P.z);
    }
  glEnd();

  struct Toe { float x, z; } toes[3] = {
    {-0.28f, 0.5f}, {0.0f, 0.6f}, {0.28f, 0.5f}
  };

  for (int i = 0; i < 3; ++i) {
    auto& toe = toes[i];
    glPushMatrix();
      // ✔️ 중심 맞춰서 위로 살짝 띄우기 (돔 높이 고려)
      glTranslatef(toe.x, 0.13f, -toe.z * 0.5f); // Y: 둥근 돔 위 살짝, Z: 약간 뒤로
  
      // ✔️ 아래 반구 제거: y < -0.13 절단
      GLdouble eqn[] = { 0.0, 1.0, 0.0, 0.13 };
      glClipPlane(GL_CLIP_PLANE0, eqn);
      glEnable(GL_CLIP_PLANE0);
  
      // ✔️ 좌우 발가락과 중심 발가락 스케일 차이
      if (i == 1) glScalef(1.1f, 1.0f, 1.4f);  // 가운데: 더 넓고 길게
      else        glScalef(1.0f, 1.0f, 1.2f);  // 양 옆: 기본
  
      // ✔️ 크기 확대 반영한 반구 발가락
      glutSolidSphere(0.25, 24, 24); // 기존 0.025 → 0.25
      // 발가락 밑면 닫기
// glBegin(GL_TRIANGLE_FAN);
// glNormal3f(0, -1, 0);
// glVertex3f(0, -0.013f, 0);  // 중심
// for (int i = 0; i <= 24; ++i) {
//   float theta = 2.0f * M_PI * i / 24;
//   float x = 0.025f * cosf(theta);  // 발가락 반지름
//   float z = 0.025f * sinf(theta);
//   glVertex3f(x, -0.013f, z);
// }
glEnd();
  
      glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
  }

  glPushMatrix();
  glTranslatef(0.0f, 0.17f, 0.3f);
  glRotatef(-90, 1, 0, 0);
  GLUquadric* quad = gluNewQuadric();
  gluQuadricNormals(quad, GLU_SMOOTH);
  gluCylinder(quad, 0.4, 0.4, 3.0, 256, 256);
  gluDeleteQuadric(quad);
  glPopMatrix();

  glPopMatrix();
}




GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if (!image) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 텍스처 파라미터
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 이미지 업로드
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    return texture;
}

// 조명
// void initLighting() {
//     GLfloat pos[] = {1, 1, 2, 1};
//     GLfloat amb[] = {0.2, 0.2, 0.2, 1};
//     GLfloat diff[] = {0.9, 0.9, 0.9, 1};
//     glLightfv(GL_LIGHT0, GL_POSITION, pos);
//     glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
//     glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
// }

// 엄청 밝은 조명
// void initLighting() {
//     GLfloat pos[] = {0, 0, 1, 0};         // w = 0 → 방향성 광원 (무한히 멀리서 균일하게 비추는 빛)
//     GLfloat amb[] = {0.9, 0.9, 0.9, 1};   // 거의 흰색 주변광 → 전체를 같은 밝기로
//     GLfloat diff[] = {0.0, 0.0, 0.0, 1};  // 확산광 제거

//     glLightfv(GL_LIGHT0, GL_POSITION, pos);
//     glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
//     glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);

//     // 🔒 감쇠 제거 → 어차피 방향성 광원이므로 필요 없음
//     glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
//     glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
//     glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
// }

//적절한 조명
void initLighting() {
    GLfloat pos[] = {0.0f, 3.5f, 1.3f, 0.0f};   // 위쪽에서 약간 앞쪽 방향 (자연광 느낌)
    GLfloat amb[] = {0.7f, 0.7f, 0.7f, 1.7f};   // 주변광: 충분히 밝지만 너무 세지 않게
    GLfloat diff[] = {0.2f, 0.2f, 0.2f, 1.0f};  // 확산광: 입체감 살림

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);

    // 감쇠 최소화 → 거리에 관계없이 거의 균일하게
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.005f);
}

// 🔧 색상 지정 함수
void setColor(float r, float g, float b) { glColor3f(r, g, b); }

// 접힌 귀
void drawFoldedEar(float x, float y, float z, float angle) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0, 0, 1);
    glScalef(0.05, 0.06, 0.11);
    setColor(1.0, 1.0, 1.0);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();

    float num;
    if (x < 0) {
        num = -x;
    } else {
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

void drawEyebrow(float x, float y, float z, float angle) {
    glPushMatrix();
    if (x > 0) {
        glTranslatef(x - 0.061, y, z);
        glRotatef(30, 0, 1, 0);
        glTranslatef(-(x - 0.061), -y, -z);
    } else {
        glTranslatef(x + 0.061, y, z);
        glRotatef(-30, 0, 1, 0);
        glTranslatef(-(x + 0.061), -y, -z);
    }
    for (int i = 1; i < 7; i++) {
        glPushMatrix();
        glTranslatef(x, y + (0.01 * i), z);                       // 위치
        glRotatef(angle, 0, 0, 1);                                // 좌우 방향 회전
        glScalef(0.15 - (0.016 * i), 0.02 + (0.002 * i), 0.025);  // 완만하고 옆으로 긴 눈썹
        setColor(0, 0, 0);
        glutSolidSphere(1.0, 10, 10);  // 좀 더 매끄러운 구체로
        glPopMatrix();
    }
    float xOffset = (x > 0) ? -0.061 : +0.061;
    for (int i = 6; i > 0; i--) {
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

void drawSnout() {
    glPushMatrix();
    glTranslatef(0.0, -0.34, 0.2);  // ⬅️ 더 아래로, 덜 튀어나오게
    glScalef(0.25, 0.18, 0.22);
    setColor(1.0, 1.0, 1.0);
    glutSolidSphere(1.0, 24, 24);
    glPopMatrix();
}
// 코: 둥근 역삼각형 느낌 (좀 더 튀어나옴)
void drawNose() {
    glPushMatrix();
    glTranslatef(0, -0.28, 0.40);
    glScalef(0.6, 0.4, 0.3);
    setColor(0, 0, 0);
    glutSolidSphere(0.135, 20, 20);  // 검은 코
    glPopMatrix();

    // 🌟 하이라이트 추가 (작은 흰색 구체를 코 위에 겹치기)
    glPushMatrix();
    glTranslatef(0.04, -0.28, 0.418);  // 살짝 오른쪽 위쪽 앞으로
    setColor(1.0, 1.0, 1.0);
    glutSolidSphere(0.02, 12, 12);  // 작고 부드러운 하이라이트
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// 'ㅅ' 형태의 입 구현
void drawMouth() {
    // 왼쪽 곡선
    glPushMatrix();
    glDisable(GL_LIGHTING);            // ← 조명 끄기
    glTranslatef(0.033, -0.41, 0.40);  // 중간이 y좌표, 마지막이 z좌표?
    glRotatef(60, 0.5, 0, 0.8);
    glScalef(0.02, 0.12, 0.005);
    setColor(0.5f, 0.5f, 0.5f);  // 중간 회색
    glutSolidCube(1.0);
    glEnable(GL_LIGHTING);  // ← 다시 켜기
    glPopMatrix();

    // 오른쪽 곡선
    glPushMatrix();
    glDisable(GL_LIGHTING);  // ← 조명 끄기
    glTranslatef(-0.033, -0.41, 0.40);
    glRotatef(-60, -0.5, 0, 0.8);
    glScalef(0.02, 0.12, 0.005);
    setColor(0.5f, 0.5f, 0.5f);  // 중간 회색
    glutSolidCube(1.0);
    glEnable(GL_LIGHTING);  // ← 다시 켜기
    glPopMatrix();

    // 가운데 세로선
    glPushMatrix();
    glDisable(GL_LIGHTING);  // ← 조명 끄기
    glTranslatef(0, -0.35, 0.42);
    glRotatef(0, 100, 100, 100);
    glScalef(0.02, 0.05, 0.005);
    setColor(0.5f, 0.5f, 0.5f);  // 중간 회색
    glutSolidCube(1.0);
    glEnable(GL_LIGHTING);  // ← 다시 켜기
    glPopMatrix();
}

// 눈(텍스처 매핑)
void drawEyeOnHeadSphere(float xOffset, float yOffset, float zOffset) {
    const float radius = 0.4f;              // 얼굴 구체의 반지름에 맞춤
    const float thetaStart = 0.35f * M_PI;  // 수직 (상하) 범위
    const float thetaEnd = 0.50f * M_PI;

    const float phiStart = 0.20f * M_PI;  // 수평 (좌우) 범위 → 양쪽 눈 전체
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
    glRotatef(90, 1.0f, 0.0f, 0.0f);  // ← 여기 추가! z축을 y축으로 회전
    for (int i = 0; i < stacks; ++i) {
        float theta1 = thetaStart + (thetaEnd - thetaStart) * i / stacks;
        float theta2 = thetaStart + (thetaEnd - thetaStart) * (i + 1) / stacks;
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float phi = phiStart + (phiEnd - phiStart) * j / slices;
            for (int k = 0; k < 2; ++k) {
                float theta = (k == 0) ? theta1 : theta2;
                float x = radius * sin(theta) * cos(phi);
                float y = radius * sin(theta) * sin(phi);
                float z = radius * cos(theta);
                float u = (phi - phiStart) / (phiEnd - phiStart);                 // 좌우
                float v = 1.0f - (theta - thetaStart) / (thetaEnd - thetaStart);  // 위아래 (상하반전)
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

// 얼굴 전체 함수
void drawKKHead() {
    glPushMatrix();                  // 얼굴 전체 이동용
    glTranslatef(0.0f, 1.4f, 0.0f);  // ⬅️ y축 위로 이동
    glScalef(2.2, 2.2, 2.2);         // 조금 납작하게
    glEnable(GL_LIGHTING);
    GLUquadric* quad = gluNewQuadric();
    int num = -1;
    while (num > -128) {
        glPushMatrix();
        setColor(1.0, 1.0, 1.0);
        glTranslatef(0.0, 0.0018 * num, 0);  // 아래로 조금 내림
        glScalef(1.0, 1.0, 1.0);             // 조금 납작하게
        gluSphere(quad, 0.4 + 0.0009 * num, 64, 64);
        glPopMatrix();
        num--;
    }
    gluDeleteQuadric(quad);  // 두 구체 다 그리고 나서 삭제

    drawFoldedEar(-0.38, 0.2, -0.05, 50);
    drawFoldedEar(0.38, 0.2, -0.05, -50);

    drawEyebrow(-0.23, 0.01, 0.35, 20);
    drawEyebrow(0.23, 0.01, 0.35, -20);
    drawEyeOnHeadSphere(0.0f, -0.03f, -0.0f);

    drawSnout();
    drawNose();
    drawMouth();
    glPopMatrix();  // 얼굴 전체 이동 마무리
}

// 디스플레이 콜백
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0, 0, zoom);
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);

    drawKKHead();
    drawBody();  // 몸통 그리기
     
    
    // 양쪽 다리 그리기
    glPushMatrix();
    glTranslatef(-0.3f, -2.0f, 0.1f);
    glRotatef(180, 0, 1, 0);       
    glScalef(0.55f, 0.55f, 0.55f);     
    drawFootWithToes(0.0f);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0.3f, -2.0f, 0.1f); 
    glRotatef(180, 0, 1, 0);   
    glScalef(0.55f, 0.55f, 0.55f);     
    drawFootWithToes(0.0f);
    glPopMatrix();

    glutSwapBuffers();
}
// 마우스 회전
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        isDragging = (state == GLUT_DOWN);
        prevX = x;
        prevY = y;
    }
}

void mouseMotion(int x, int y) {
    if (isDragging) {
        angleY += (x - prevX) * 0.5;
        angleX += (y - prevY) * 0.5;
        prevX = x;
        prevY = y;
        glutPostRedisplay();
    }
}
// 키보드 줌
void keyboard(unsigned char key, int x, int y) {
    if (key == 'a') zoom += 0.3f;
    if (key == 'z') zoom -= 0.3f;
    glutPostRedisplay();
}

// 리사이즈
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (double)width / height, 1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char* argv[]) {
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
    eyeTexture = loadTexture("kk_eye1.png");
    initLighting();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;

}
