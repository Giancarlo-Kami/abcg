#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <random>

#include "abcg.hpp"
#include "model.hpp"
#include "trackball.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:

  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};


  TrackBall m_trackBallLight;
  std::default_random_engine m_randomEngine;

  Model m_model;

  glm::vec3 m_position{0.0f,0.0f,0.0f};

  glm::vec3 m_velocity{1.0f,1.0f,1.0f};
  float m_escalar = 1.0f;

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  glm::mat4 m_modelMatrix{1.0f};

  glm::vec4 m_lightDir{-1.0f, -1.0f, 1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Id{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Is{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Ka{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_Kd{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_Ks{1.0f, 1.0f, 1.0f, 1.0f};
  float m_shininess{25.0f};

  void loadModel(std::string_view path);
  void update();
};

#endif