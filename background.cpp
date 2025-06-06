#include "stb_image.h"
#include <GL/glut.h>
#include "stb_image.h"

GLuint textures[6];
extern float angleX, angleY;

GLuint loadSkyboxTexture(const char *filename)
{
    int width, height, channels;
    unsigned char *image = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    if (!image)
    {
        printf("Failed to load: %s\n", filename);
        return 0;
    }
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    return texID;
}

void drawQuad(float size)
{
    float texScale = 1.0f;
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-size, -size, size);
    glTexCoord2f(texScale, 0);
    glVertex3f(size, -size, size);
    glTexCoord2f(texScale, texScale);
    glVertex3f(size, size, size);
    glTexCoord2f(0, texScale);
    glVertex3f(-size, size, size);
    glEnd();
}

void drawSkybox()
{
    static bool loaded = false;
    if (!loaded)
    {
        textures[0] = loadSkyboxTexture("Image/background/front.png");
        textures[1] = loadSkyboxTexture("Image/background/back1.png");
        textures[2] = loadSkyboxTexture("Image/background/left1.png");
        textures[3] = loadSkyboxTexture("Image/background/right1.png");
        textures[4] = loadSkyboxTexture("Image/background/top.png");
        textures[5] = loadSkyboxTexture("Image/background/bottom.png");
        loaded = true;
    }

    float size = 2.0f; // 크기 증가
    float offset = 0.0f;

    // float size = 200.0f;
    // float half = size / 2.0f;
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // ✅ 내부에서 보이게
    glEnable(GL_TEXTURE_2D);

    // ✅ 카메라 위치 기준으로 skybox 고정
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);
    // +Z (Front)
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glPushMatrix();
    glTranslatef(0, 0, offset);
    glScalef(-1, -1, 1); // ✅ Y축 반전 추가
    drawQuad(size);
    glPopMatrix();

    // -Z (Back)
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glPushMatrix();
    glTranslatef(0, 0, -offset);
    glRotatef(180, 0, 1, 0);
    glScalef(-1, -1, 1); // ✅ Y축 반전 추가
    drawQuad(size);
    glPopMatrix();

    // -X (Left)
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glPushMatrix();
    glTranslatef(-offset, 0, 0);
    glRotatef(-90, 0, 1, 0);
    glScalef(-1, -1, 1); // ✅ Y축 반전 추가
    drawQuad(size);
    glPopMatrix();

    // +X (Right)
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glPushMatrix();
    glTranslatef(offset, 0, 0);
    glRotatef(90, 0, 1, 0);
    glScalef(-1, -1, 1); // ✅ Y축 반전 추가
    drawQuad(size);
    glPopMatrix();

    // +Y (Top)
    glBindTexture(GL_TEXTURE_2D, textures[4]);
    glPushMatrix();
    glTranslatef(0, offset, 0);
    glRotatef(-90, 1, 0, 0);
    drawQuad(size);
    glPopMatrix();

    // -Y (Bottom)
    glBindTexture(GL_TEXTURE_2D, textures[5]);
    glPushMatrix();
    glTranslatef(0, -offset, 0);
    glRotatef(90, 1, 0, 0);
    drawQuad(size);
    glPopMatrix();

    glPopMatrix(); // MODELVIEW
    glPopAttrib(); // 상태 복원
}
void drawGround()
{
    static GLuint groundTex = 0;
    static bool groundLoaded = false;

    if (!groundLoaded)
    {
        groundTex = loadSkyboxTexture("Image/background/field.png"); // ✅ 상대경로 확인
        if (groundTex == 0)
        {
            printf("Failed to load field.png only once\n");
            return;
        }
        groundLoaded = true;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, groundTex);

    glPushMatrix();
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    float size = 20.0f;
    float size1 = 30.0f;
    glTexCoord2f(0, 0);
    glVertex3f(-size1, -1.9f, -size);
    glTexCoord2f(1, 0);
    glVertex3f(size1, -1.9f, -size);
    glTexCoord2f(1, 1);
    glVertex3f(size1, -1.9f, size);
    glTexCoord2f(0, 1);
    glVertex3f(-size1, -1.9f, size);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}
