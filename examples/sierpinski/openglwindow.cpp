#include "openglwindow.hpp"
#include <fmt/core.h>
#include <imgui.h>
#include <chrono>

void OpenGLWindow::initializeGL() {
  const auto *vertexShader{R"gl(
    #version 410
    layout(location = 0) in vec2 inPosition;
    void main() { 
      gl_PointSize = 2.0;
      gl_Position = vec4(inPosition, 0, 1); 
    }
  )gl"};

  const auto *fragmentShader{R"gl(
    #version 410
    out vec4 outColor;
    void main() { outColor = vec4(1); }
  )gl"};

  // Create shader program
  m_program = createProgramFromString(vertexShader, fragmentShader);

  // Clear window
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  std::array<GLfloat, 2> sizes{};
#if !defined(EMSCRIPTEN)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
  abcg::glGetFloatv(GL_POINT_SIZE_RANGE, sizes.data());
#else
  abcg::glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, sizes.data());
#endif
  fmt::print("Point size: {:.2f} (min), {:.2f} (max)\n", sizes[0], sizes[1]);

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  // Randomly choose a pair of coordinates in the interval [-1; 1]
  std::uniform_real_distribution<float> realDistribution(-1.0f, 1.0f);
  m_P.x = realDistribution(m_randomEngine);
  m_P.y = realDistribution(m_randomEngine);
}

void OpenGLWindow::paintGL() {
  // Create OpenGL buffers for the single point at m_P
  setupModel();

  // Set the viewport
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // Start using the shader program
  abcg::glUseProgram(m_program);
  // Start using VAO
  abcg::glBindVertexArray(m_vao);

  // Draw a single point
  abcg::glDrawArrays(GL_POINTS, 0, 1);

  // End using VAO
  abcg::glBindVertexArray(0);
  // End using the shader program
  abcg::glUseProgram(0);

  // Randomly choose a triangle vertex index
  std::uniform_int_distribution<int> intDistribution(0, m_points.size() - 1);
  const int index{intDistribution(m_randomEngine)};

  // The new position is the midpoint between the current position and the
  // chosen vertex
  m_P = (m_P + m_points.at(index)) / 2.0f;

  // Print coordinates to the console
  // fmt::print("({:+.2f}, {:+.2f})\n", m_P.x, m_P.y);
}

void OpenGLWindow::setupModel() {
  // Release previous VBO and VAO
  abcg::glDeleteBuffers(1, &m_vboVertices);
  abcg::glDeleteVertexArrays(1, &m_vao);

  // Generate a new VBO and get the associated ID
  abcg::glGenBuffers(1, &m_vboVertices);
  // Bind VBO in order to use it
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
  // Upload data to VBO
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_P), &m_P, GL_STATIC_DRAW);
  // Unbinding the VBO is allowed (data can be released now)
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const GLint positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void OpenGLWindow::resizeGL(int width, int height){
    m_viewportWidth = width;
    m_viewportHeight = height;

    abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL(){
    abcg::glDeleteProgram(m_program);
    abcg::glDeleteBuffers(1,&m_vboVertices);
    abcg::glDeleteVertexArrays(1,&m_vao);
}

void OpenGLWindow::paintUI(){
    abcg::OpenGLWindow::paintUI();
    {
        ImGui::SetNextWindowPos(ImVec2(5,81));
        ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoDecoration);

        if(ImGui::Button("Clear window", ImVec2(150,30))){
            abcg::glClear(GL_COLOR_BUFFER_BIT);
        }

        ImGui::End();
    }
}

