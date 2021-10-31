  #ifndef TARGETS_HPP_
  #define TARGETS_HPP_

  #include <list>
  #include <random>

  #include "abcg.hpp"
  #include "gamedata.hpp"
  #include "tank.hpp"

  class OpenGLWindow;

  class Targets{
      public:
        void initializeGL(GLuint program, int qtd);
        void paintGL();
        void terminateGL();

        void update( float deltaTime);

    private:
        friend OpenGLWindow;

        GLuint m_program{};
        GLint m_colorLoc{};
        GLint m_translationLoc{};
        GLint m_scaleLoc{};

        struct Target {
            GLuint m_vao{};
            GLuint m_vbo{};

            glm::vec4 m_color{1,0,0,1};
            bool m_hit{false};
            int m_polygonSides{50};
            float m_scale;
            glm::vec2 m_translation{glm::vec2(0)};
        };
        
        std::list<Target> m_targets;

        std::default_random_engine m_randomEngine;
        std::uniform_real_distribution<float> m_randomDist{0.0f,0.5f};

        Targets::Target createTarget(glm::vec2 translation = glm::vec2(0));
};

#endif