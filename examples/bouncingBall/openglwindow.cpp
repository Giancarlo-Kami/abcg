#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(0, 0, 0, 1);

  abcg::glEnable(GL_DEPTH_TEST);

  m_program = createProgramFromFile(getAssetsPath() + "depth.vert", getAssetsPath() + "depth.frag");

  m_model.loadObj(getAssetsPath() + "sphere.obj");

  m_model.setupVAO(m_program);

  m_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  m_position = glm::vec3(0.0f,0.0f,-10.0f);
}


void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  abcg::glUseProgram(m_program);

  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
    if(m_velocity[2] > 0)
  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
    else
  abcg::glUniform4f(colorLoc, 0.6f, 0.81f, 0.51f, 1.0f);
  glm::mat4 modelMatrix{1.0f};
  modelMatrix = glm::translate(modelMatrix, m_position);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(m_position.z/2.5f));
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
  m_model.render();
  
  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  std::uniform_real_distribution<float> vel(-1.0f, 1.0f);

  {
    ImGui::SetNextWindowPos(ImVec2(5, m_viewportHeight - 94));
    ImGui::SetNextWindowSize(ImVec2(m_viewportWidth - 10, -1));
    ImGui::Begin("Slider window", nullptr, ImGuiWindowFlags_NoDecoration);
    {
      ImGui::PushItemWidth(m_viewportWidth - 25);
      ImGui::SliderFloat("", &m_escalar, 1.0f, 30.0f, "velocidade: %f ");
      ImGui::PopItemWidth();
    }
    ImGui::End();
  }

  {
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth-210,1));
    ImGui::SetNextWindowSize(ImVec2(200, 90));
    ImGui::Begin("Speed button", nullptr, ImGuiWindowFlags_NoDecoration);

    if (ImGui::Button("Random Speed", ImVec2(-1, 30))) {
      m_velocity = glm::vec3(1.0f,1.0f,1.0f);
      m_velocity = glm::vec3(m_velocity.x *vel(m_randomEngine), m_velocity.y *vel(m_randomEngine), m_velocity.z *vel(m_randomEngine) );
    }
    ImGui::Text("Vetor Unitario:");
    ImGui::Text("x = %.2f y=%.2f z=%.2f",m_velocity.x,m_velocity.y,m_velocity.z);

    ImGui::End();
  }

  const auto aspect{static_cast<float>(m_viewportWidth) / static_cast<float>(m_viewportHeight)};
  m_projMatrix = glm::ortho(-20.0f * aspect, 20.0f * aspect, -20.0f, 20.0f, 0.01f, 100.0f);
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
}

void OpenGLWindow::terminateGL() {
  m_model.terminateGL();
  abcg::glDeleteProgram(m_program);
}

void OpenGLWindow::update() {
  
  const float deltaTime{static_cast<float>(getDeltaTime())};

  if((m_position.x > 19.9f && m_velocity[0] > 0)|| (m_position.x < -19.9f && m_velocity[0] < 0))
    m_velocity[0] = -m_velocity[0];
  if((m_position.y > 19.9f && m_velocity[1] > 0)|| (m_position.y < -19.9f && m_velocity[1] < 0))
    m_velocity[1] = -m_velocity[1];
  if ((m_position.z > 0.1f && m_velocity[2] > 0)|| (m_position.z < -20.0f && m_velocity[2] < 0)) 
      m_velocity[2] = -m_velocity[2];

    m_position += deltaTime * (m_velocity * m_escalar);
}
