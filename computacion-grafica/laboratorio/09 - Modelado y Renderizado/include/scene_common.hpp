#pragma once

#include <GL/glut.h>

#include <cmath>
#include <string>

namespace scene
{

    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kTerrainHalfSize = 20.0f; // Terreno 40x40 centrado en el origen.

    enum class RenderMode
    {
        Colors,    // Parte 01: colores con glColor3f.
        Materials, // Parte 02 y 03: materiales con glMaterial*.
        Textures,  // Parte 04: materiales + texturas.
    };

    struct Material
    {
        GLfloat ambient[4];
        GLfloat diffuse[4];
        GLfloat specular[4];
        GLfloat shininess;
    };

    struct TextureBundle
    {
        GLuint terrain = 0;
        GLuint walls = 0;
        GLuint roof = 0;
        GLuint trunk = 0;
        GLuint canopy = 0;
    };

    struct CameraState
    {
        float yaw = 35.0f;
        float pitch = 22.0f;
        float distance = 48.0f;
    };

    inline Material makeMaterial(
        float ar,
        float ag,
        float ab,
        float dr,
        float dg,
        float db,
        float sr,
        float sg,
        float sb,
        float shininess)
    {
        return {{ar, ag, ab, 1.0f}, {dr, dg, db, 1.0f}, {sr, sg, sb, 1.0f}, shininess};
    }

    inline const Material &terrainMaterial()
    {
        static const Material material = makeMaterial(
            0.04f, 0.18f, 0.04f,
            0.18f, 0.58f, 0.16f,
            0.04f, 0.08f, 0.04f,
            8.0f);
        return material;
    }

    inline const Material &wallMaterial()
    {
        static const Material material = makeMaterial(
            0.30f, 0.05f, 0.04f,
            0.78f, 0.14f, 0.12f,
            0.25f, 0.15f, 0.12f,
            24.0f);
        return material;
    }

    inline const Material &roofMaterial()
    {
        static const Material material = makeMaterial(
            0.18f, 0.18f, 0.18f,
            0.55f, 0.55f, 0.55f,
            0.30f, 0.30f, 0.30f,
            18.0f);
        return material;
    }

    inline const Material &trunkMaterial()
    {
        static const Material material = makeMaterial(
            0.18f, 0.10f, 0.04f,
            0.45f, 0.25f, 0.10f,
            0.12f, 0.08f, 0.04f,
            10.0f);
        return material;
    }

    inline const Material &canopyMaterial()
    {
        static const Material material = makeMaterial(
            0.03f, 0.18f, 0.03f,
            0.14f, 0.55f, 0.14f,
            0.12f, 0.22f, 0.12f,
            20.0f);
        return material;
    }

    inline void initializeCommonState()
    {
        glClearColor(0.70f, 0.82f, 0.95f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_NORMALIZE);
        glShadeModel(GL_SMOOTH);
    }

    inline void initializeRecommendedLightingState()
    {
        glShadeModel(GL_SMOOTH);
        glFrontFace(GL_CCW);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glEnable(GL_NORMALIZE);
    }

    inline void applyProjection(int width, int height)
    {
        const int safeHeight = height == 0 ? 1 : height;
        const int safeWidth = width == 0 ? 1 : width;

        glViewport(0, 0, safeWidth, safeHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, static_cast<double>(safeWidth) / static_cast<double>(safeHeight), 1.0, 200.0);
        glMatrixMode(GL_MODELVIEW);
    }

    inline void applyCamera(const CameraState &camera)
    {
        const float yawRadians = camera.yaw * kPi / 180.0f;
        const float pitchRadians = camera.pitch * kPi / 180.0f;
        const float x = camera.distance * std::cos(pitchRadians) * std::sin(yawRadians);
        const float y = camera.distance * std::sin(pitchRadians);
        const float z = camera.distance * std::cos(pitchRadians) * std::cos(yawRadians);

        gluLookAt(x, y, z, 0.0, 4.0, 0.0, 0.0, 1.0, 0.0);
    }

    inline void adjustCameraOnArrowKey(CameraState &camera, int key)
    {
        switch (key)
        {
        case GLUT_KEY_LEFT:
            camera.yaw -= 4.0f;
            break;
        case GLUT_KEY_RIGHT:
            camera.yaw += 4.0f;
            break;
        case GLUT_KEY_UP:
            camera.pitch += 3.0f;
            break;
        case GLUT_KEY_DOWN:
            camera.pitch -= 3.0f;
            break;
        default:
            break;
        }

        if (camera.pitch > 80.0f)
        {
            camera.pitch = 80.0f;
        }
        if (camera.pitch < 5.0f)
        {
            camera.pitch = 5.0f;
        }
    }

    inline void zoomCamera(CameraState &camera, float delta)
    {
        camera.distance += delta;
        if (camera.distance < 18.0f)
        {
            camera.distance = 18.0f;
        }
        if (camera.distance > 90.0f)
        {
            camera.distance = 90.0f;
        }
    }

    inline void applyMaterial(const Material &material)
    {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material.ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material.diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material.specular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess);
    }

    inline void applyAppearance(RenderMode mode, const Material &material, float red, float green, float blue)
    {
        if (mode == RenderMode::Colors)
        {
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glColor3f(red, green, blue);
            return;
        }

        applyMaterial(material);
    }

    inline void bindTextureForObject(RenderMode mode, GLuint textureId)
    {
        if (mode == RenderMode::Textures && textureId != 0U)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
        }
    }

    inline void computeNormal(
        float ax,
        float ay,
        float az,
        float bx,
        float by,
        float bz,
        float cx,
        float cy,
        float cz)
    {
        const float ux = bx - ax;
        const float uy = by - ay;
        const float uz = bz - az;
        const float vx = cx - ax;
        const float vy = cy - ay;
        const float vz = cz - az;

        const float nx = uy * vz - uz * vy;
        const float ny = uz * vx - ux * vz;
        const float nz = ux * vy - uy * vx;
        const float length = std::sqrt(nx * nx + ny * ny + nz * nz);

        if (length > 0.0f)
        {
            glNormal3f(nx / length, ny / length, nz / length);
        }
    }

    inline void drawTerrain(RenderMode mode, const TextureBundle *textures)
    {
        bindTextureForObject(mode, textures ? textures->terrain : 0U);
        applyAppearance(mode, terrainMaterial(), 0.18f, 0.60f, 0.18f);

        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-kTerrainHalfSize, 0.0f, -kTerrainHalfSize);

        glTexCoord2f(0.0f, 8.0f);
        glVertex3f(-kTerrainHalfSize, 0.0f, kTerrainHalfSize);

        glTexCoord2f(8.0f, 8.0f);
        glVertex3f(kTerrainHalfSize, 0.0f, kTerrainHalfSize);

        glTexCoord2f(8.0f, 0.0f);
        glVertex3f(kTerrainHalfSize, 0.0f, -kTerrainHalfSize);
        glEnd();
    }

    inline void drawHouse(RenderMode mode, const TextureBundle *textures)
    {
        constexpr float x0 = -5.0f;
        constexpr float x1 = 5.0f;
        constexpr float z0 = -4.0f;
        constexpr float z1 = 4.0f;
        constexpr float yWall = 5.0f;
        constexpr float yRoof = 8.0f;

        bindTextureForObject(mode, textures ? textures->walls : 0U);
        applyAppearance(mode, wallMaterial(), 0.78f, 0.16f, 0.14f);

        glBegin(GL_QUADS);
        // Frente.
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x0, 0.0f, z1);
        glTexCoord2f(2.0f, 0.0f);
        glVertex3f(x1, 0.0f, z1);
        glTexCoord2f(2.0f, 1.5f);
        glVertex3f(x1, yWall, z1);
        glTexCoord2f(0.0f, 1.5f);
        glVertex3f(x0, yWall, z1);

        // Posterior.
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x1, 0.0f, z0);
        glTexCoord2f(2.0f, 0.0f);
        glVertex3f(x0, 0.0f, z0);
        glTexCoord2f(2.0f, 1.5f);
        glVertex3f(x0, yWall, z0);
        glTexCoord2f(0.0f, 1.5f);
        glVertex3f(x1, yWall, z0);

        // Izquierda.
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x0, 0.0f, z0);
        glTexCoord2f(1.6f, 0.0f);
        glVertex3f(x0, 0.0f, z1);
        glTexCoord2f(1.6f, 1.5f);
        glVertex3f(x0, yWall, z1);
        glTexCoord2f(0.0f, 1.5f);
        glVertex3f(x0, yWall, z0);

        // Derecha.
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x1, 0.0f, z1);
        glTexCoord2f(1.6f, 0.0f);
        glVertex3f(x1, 0.0f, z0);
        glTexCoord2f(1.6f, 1.5f);
        glVertex3f(x1, yWall, z0);
        glTexCoord2f(0.0f, 1.5f);
        glVertex3f(x1, yWall, z1);
        glEnd();

        // Triángulos frontales y posteriores del techo/cierre superior: rojos / ladrillo.
        glBegin(GL_TRIANGLES);
        computeNormal(x0, yWall, z1, x1, yWall, z1, 0.0f, yRoof, z1);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x0, yWall, z1);
        glTexCoord2f(2.0f, 0.0f);
        glVertex3f(x1, yWall, z1);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.0f, yRoof, z1);

        computeNormal(x1, yWall, z0, x0, yWall, z0, 0.0f, yRoof, z0);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x1, yWall, z0);
        glTexCoord2f(2.0f, 0.0f);
        glVertex3f(x0, yWall, z0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.0f, yRoof, z0);
        glEnd();

        bindTextureForObject(mode, textures ? textures->roof : 0U);
        applyAppearance(mode, roofMaterial(), 0.60f, 0.60f, 0.60f);

        glBegin(GL_QUADS);
        computeNormal(x0, yWall, z1, 0.0f, yRoof, z1, 0.0f, yRoof, z0);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x0, yWall, z1);
        glTexCoord2f(1.4f, 0.0f);
        glVertex3f(0.0f, yRoof, z1);
        glTexCoord2f(1.4f, 2.4f);
        glVertex3f(0.0f, yRoof, z0);
        glTexCoord2f(0.0f, 2.4f);
        glVertex3f(x0, yWall, z0);

        computeNormal(0.0f, yRoof, z1, x1, yWall, z1, x1, yWall, z0);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0.0f, yRoof, z1);
        glTexCoord2f(1.4f, 0.0f);
        glVertex3f(x1, yWall, z1);
        glTexCoord2f(1.4f, 2.4f);
        glVertex3f(x1, yWall, z0);
        glTexCoord2f(0.0f, 2.4f);
        glVertex3f(0.0f, yRoof, z0);
        glEnd();
    }

    inline void drawTree(RenderMode mode, const TextureBundle *textures)
    {
        GLUquadric *quadric = gluNewQuadric();
        if (!quadric)
        {
            return;
        }

        gluQuadricOrientation(quadric, GLU_OUTSIDE);
        gluQuadricNormals(quadric, GLU_SMOOTH);
        gluQuadricTexture(quadric, mode == RenderMode::Textures ? GL_TRUE : GL_FALSE);

        glPushMatrix();
        glTranslatef(11.0f, 0.0f, 8.0f);

        // Tronco.
        bindTextureForObject(mode, textures ? textures->trunk : 0U);
        applyAppearance(mode, trunkMaterial(), 0.45f, 0.26f, 0.10f);
        glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        gluCylinder(quadric, 0.75, 0.60, 5.5, 32, 8);
        gluDisk(quadric, 0.0, 0.75, 32, 1);
        glTranslatef(0.0f, 0.0f, 5.5f);
        gluDisk(quadric, 0.0, 0.60, 32, 1);
        glPopMatrix();

        // Copa.
        bindTextureForObject(mode, textures ? textures->canopy : 0U);
        applyAppearance(mode, canopyMaterial(), 0.18f, 0.65f, 0.18f);
        glTranslatef(0.0f, 6.7f, 0.0f);
        gluSphere(quadric, 2.6, 32, 24);

        glPopMatrix();
        gluDeleteQuadric(quadric);
    }

    inline void drawScene(RenderMode mode, const TextureBundle *textures = nullptr)
    {
        drawTerrain(mode, textures);
        drawHouse(mode, textures);
        drawTree(mode, textures);
        glDisable(GL_TEXTURE_2D);
    }

    inline void drawLightMarker(float x, float y, float z)
    {
        GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
        GLboolean textureWasEnabled = glIsEnabled(GL_TEXTURE_2D);

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 0.92f, 0.15f);

        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(0.45, 16, 12);
        glPopMatrix();

        if (lightingWasEnabled)
        {
            glEnable(GL_LIGHTING);
        }
        if (textureWasEnabled)
        {
            glEnable(GL_TEXTURE_2D);
        }
    }

    inline void drawBitmapText(float x, float y, const std::string &text)
    {
        glRasterPos2f(x, y);
        for (const unsigned char character : text)
        {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, character);
        }
    }

    inline void drawOverlay(const std::string &title, const std::string &controls, const std::string &details)
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, 1000.0, 0.0, 1000.0);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
        GLboolean textureWasEnabled = glIsEnabled(GL_TEXTURE_2D);

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.08f, 0.08f, 0.08f);
        drawBitmapText(24.0f, 962.0f, title);
        drawBitmapText(24.0f, 934.0f, controls);
        drawBitmapText(24.0f, 906.0f, details);

        if (lightingWasEnabled)
        {
            glEnable(GL_LIGHTING);
        }
        if (textureWasEnabled)
        {
            glEnable(GL_TEXTURE_2D);
        }

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

}
