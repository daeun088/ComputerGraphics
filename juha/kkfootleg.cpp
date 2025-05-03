// main.cpp - 양발/양다리 구조 구현 (10배 확대 버전)
#include <GL/glut.h>
#include <vector>
#include <array>
#include <cmath>

float angleX = 20, angleY = -30, zoom = -12.0f;

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
  gluCylinder(quad, 0.4, 0.4, 1.9, 256, 256);
  gluDeleteQuadric(quad);
  glPopMatrix();

  glPopMatrix();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0, 0, zoom);
  glRotatef(angleX, 1, 0, 0);
  glRotatef(angleY, 0, 1, 0);
  drawFootWithToes(-0.8f);
  drawFootWithToes( 0.8f);
  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45,(float)w/h,0.1,50);
  glMatrixMode(GL_MODELVIEW);
}

void mouseMotion(int x,int y){ static int lx=-1,ly=-1;
  if(lx>=0){ angleY+=(x-lx)*0.5f; angleX+=(y-ly)*0.5f; }
  lx=x; ly=y; glutPostRedisplay();
}
void mouseWheel(int b,int s,int x,int y){
  if(b==3) zoom+=0.1f; else if(b==4) zoom-=0.1f;
  glutPostRedisplay();
}
void keyboard(unsigned char k,int x,int y){
  if(k=='+'||k=='=') zoom+=0.1f;
  if(k=='-'||k=='_') zoom-=0.1f;
  glutPostRedisplay();
}
void special(int k,int x,int y){
  if(k==GLUT_KEY_UP) zoom+=0.1f;
  if(k==GLUT_KEY_DOWN) zoom-=0.1f;
  glutPostRedisplay();
}

int main(int c,char**v){
  glutInit(&c,v);
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(600,400);
  glutCreateWindow("Two Legs with Feet (Scaled 10x)");
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.5f, 0.0f, 0.13f, 1.0f);
  GLfloat amb[]={0.2f,0.2f,0.2f,1}, dif[]={0.9f,0.9f,0.9f,1}, pos[]={-6.0f,10.0f,10.0f,0};
  glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,dif);
  glLightfv(GL_LIGHT0,GL_POSITION,pos);
  GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat spec[]  = {0.3f, 0.3f, 0.3f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16.0f);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMotionFunc(mouseMotion);
  glutMouseFunc(mouseWheel);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutMainLoop();
  return 0;
}
