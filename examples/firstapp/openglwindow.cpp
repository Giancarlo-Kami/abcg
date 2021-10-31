#include <fmt/core.h>
#include "openglwindow.hpp"
#include <imgui.h>

void OpenGLWindow::initializeGL(){
    auto windowSettings{getWindowSettings()};
    fmt::print("Initial window size: {}x{}\n", windowSettings.width, windowSettings.height);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  abcg::glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
                     m_clearColor[3]);
  // Clear the color buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::paintUI(){
    abcg::OpenGLWindow::paintUI();

    {
        ImGui::Begin("Hello, First App!");

        auto windowSettings{getWindowSettings()};
        ImGui::Text("current window size: %dx%d (in windowed mode)",windowSettings.width,windowSettings.height);

        static float f{};
        ImGui::SliderFloat("float", &f,0.0f,1.0f);

        ImGui::ColorEdit3("clear color", m_clearColor.data());

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }
}