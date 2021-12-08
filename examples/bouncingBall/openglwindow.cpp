#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtc/matrix_inverse.hpp>

void OpenGLWindow::handleEvent(SDL_Event& event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBallLight.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mousePress(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mouseRelease(mousePosition);
    }
  }
}

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(0, 0, 0.5, 0.5);

  abcg::glEnable(GL_DEPTH_TEST);

  m_program = createProgramFromFile(getAssetsPath() + "texture.vert", getAssetsPath() + "texture.frag");

  loadModel(getAssetsPath() + "sphere.obj");

  m_model.setupVAO(m_program);

  m_viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  m_position = glm::vec3(0.0f,0.0f,-10.0f);


  abcg::glFrontFace(GL_CW);
}

void OpenGLWindow::loadModel(std::string_view path) {
  m_model.terminateGL();

  m_model.loadDiffuseTexture(getAssetsPath() + "pattern.png");
  m_model.loadObj(path);
  m_model.setupVAO(m_program);

  // Use material properties from the loaded model
  m_Ka = m_model.getKa();
  m_Kd = m_model.getKd();
  m_Ks = m_model.getKs();
  m_shininess = m_model.getShininess();
}

void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  abcg::glUseProgram(m_program);

  const GLint viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint normalMatrixLoc{abcg::glGetUniformLocation(m_program, "normalMatrix")};

  const GLint lightDirLoc{abcg::glGetUniformLocation(m_program, "lightDirWorldSpace")};
  const GLint shininessLoc{abcg::glGetUniformLocation(m_program, "shininess")};
  const GLint IaLoc{abcg::glGetUniformLocation(m_program, "Ia")};
  const GLint IdLoc{abcg::glGetUniformLocation(m_program, "Id")};
  const GLint IsLoc{abcg::glGetUniformLocation(m_program, "Is")};
  const GLint KaLoc{abcg::glGetUniformLocation(m_program, "Ka")};
  const GLint KdLoc{abcg::glGetUniformLocation(m_program, "Kd")};
  const GLint KsLoc{abcg::glGetUniformLocation(m_program, "Ks")};
  const GLint diffuseTexLoc{abcg::glGetUniformLocation(m_program, "diffuseTex")};

  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);

  const auto lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  abcg::glUniform1f(shininessLoc, m_shininess);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);
  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);

  glm::mat4 modelMatrix{1.0f};
  modelMatrix = glm::translate(modelMatrix, m_position);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(m_position.z/2.5f));
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  const auto modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
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

  m_trackBallLight.resizeViewport(width, height);
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
