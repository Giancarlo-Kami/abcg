#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>
#include <random>
#include "abcg.hpp"
#include "tank.hpp"
#include "bullet.hpp"
#include "bg.hpp"
#include "targets.hpp"

class OpenGLWindow : public abcg::OpenGLWindow { 
    protected:
        void handleEvent(SDL_Event& event) override;
        void initializeGL() override;
        void paintGL() override;
        void paintUI() override;
        void resizeGL(int width, int height) override;
        void terminateGL() override;

    private:
        GLuint m_objectsProgram{};

        int m_viewportWidth{};
        int m_viewportHeight{};

        GameData m_gameData;


        
        Targets m_targets;
        Bullets m_bullet;
        Tank m_tank;

        abcg::ElapsedTimer m_restartWaitTimer;

        ImFont* m_font{};

        std::default_random_engine m_randomEngine;

        void restart();
        void update();
        void checkCollision();
        void checkWinCondition();

};

#endif