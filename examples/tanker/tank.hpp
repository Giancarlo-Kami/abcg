#ifndef SHIP_HPP_
#define SHIP_HPP_

#include "abcg.hpp"
#include "gamedata.hpp"

class Bg;
class Bullets;
class OpenGLWindow;
class Targets;

class Tank{
    public:
        void initializeGL(GLuint program);
        void paintGL(const GameData &gameData);
        void terminateGL();

        void setAngle(float angle) { m_angle = angle;}

    private:
        friend Bg;
        friend Bullets;
        friend OpenGLWindow;
        friend Targets;

        GLuint m_program{};
        GLint m_colorLoc{};
        GLint m_scaleLoc{};
        GLint m_angleLoc{};

        GLuint m_vao{};
        GLuint m_vbo{};
        GLuint m_ebo{};

        glm::vec4 m_color{0,0.4f,0.01f,1};
        float m_angle{};
        float m_scale{0.125f};

        abcg::ElapsedTimer m_bulletCoolDownTimer;
};
#endif