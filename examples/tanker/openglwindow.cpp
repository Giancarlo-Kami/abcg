#include "openglwindow.hpp"
#include <imgui.h>
#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event){
    if(event.type == SDL_KEYDOWN){
        if(event.key.keysym.sym == SDLK_SPACE)
            m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
        if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
            m_gameData.m_input.set(static_cast<size_t>(Input::Up));
        if(event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
            m_gameData.m_input.set(static_cast<size_t>(Input::Down));
    }
    if(event.type == SDL_KEYUP){
        if(event.key.keysym.sym == SDLK_SPACE)
            m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
        if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
            m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
        if(event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
            m_gameData.m_input.reset(static_cast<size_t>(Input::Down));
    }

    if(event.type == SDL_MOUSEBUTTONDOWN) {
        if(event.button.button == SDL_BUTTON_LEFT)
            m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
    }
    if(event.type == SDL_MOUSEBUTTONUP) {
        if(event.button.button == SDL_BUTTON_LEFT)
            m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    }
    if(event.type == SDL_MOUSEMOTION){
        glm::ivec2 mousePosition;
        SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

        m_tank.setAngle(std::cos(glm::distance(glm::vec2{mousePosition.x,mousePosition.y},glm::vec2{-0.8f,-0.8f})/ glm::distance(glm::vec2{mousePosition.x, -0.8f}, glm::vec2(-0.8f,-0.8f))));
    }
}

void OpenGLWindow::initializeGL(){
    ImGuiIO &io{ImGui::GetIO()};
    auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
    m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(),60.0f);
    if(m_font == nullptr){
        throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
    }

    m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",getAssetsPath() + "objects.frag" );

    abcg::glClearColor(0.2f,0.8f,0.8f,1);

    #if !defined(__EMSCRIPTEN__)
        abcg::glEnable(GL_PROGRAM_POINT_SIZE);
    #endif

    m_randomEngine.seed(std::chrono::steady_clock::now().time_since_epoch().count());

    restart();
}

void OpenGLWindow::restart(){
    m_gameData.m_state = State::Playing;

    m_targets.initializeGL(m_objectsProgram, 3);
    m_tank.initializeGL(m_objectsProgram);
    m_bullet.initializeGL(m_objectsProgram);
}

void OpenGLWindow::update(){
    float deltaTime{static_cast<float>(getDeltaTime())};

    if(m_gameData.m_state != State::Playing && m_restartWaitTimer.elapsed()>5){
        restart();
        return;
    }

    m_targets.update(deltaTime);
    m_bullet.update(m_tank, m_gameData, deltaTime);

    if(m_gameData.m_state == State::Playing){
        checkCollision();
        checkWinCondition();
    }
}

void OpenGLWindow::paintGL(){
    update();

    abcg::glClear(GL_COLOR_BUFFER_BIT);
    abcg::glViewport(0,0,m_viewportWidth,m_viewportHeight);

    
    m_tank.paintGL(m_gameData);
    m_targets.paintGL();
    m_bullet.paintGL();
}

void OpenGLWindow::paintUI(){
    abcg::OpenGLWindow::paintUI();

    {
        const auto size{ImVec2(300,85)};
        const auto position{ImVec2((m_viewportWidth - size.x)/2.0f, (m_viewportHeight - size.y)/2.0f)};
        ImGui::SetNextWindowPos(position);
        ImGui::SetNextWindowSize(size);
        ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs};
        ImGui::Begin(" ", nullptr, flags);
        ImGui::PushFont(m_font);

        if(m_gameData.m_state == State::GameOver){
            ImGui::Text("Game Over");
        }else if(m_gameData.m_state == State::Win){
            ImGui::Text("You Win!");
        }

        ImGui::PopFont();
        ImGui::End();
    }
}

void OpenGLWindow::resizeGL(int width, int height){
    m_viewportWidth = width;
    m_viewportHeight = height;

    abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL(){
    abcg::glDeleteProgram(m_objectsProgram);
    m_targets.terminateGL();
    m_tank.terminateGL();
    m_bullet.terminateGL();
}

void OpenGLWindow::checkCollision(){
    for(const auto &bullet : m_bullet.m_bullet){
        if(bullet.m_dead) continue;

        for(auto &target : m_targets.m_targets){
            const auto distance{glm::distance(bullet.m_translation,target.m_translation)};
            if(distance < m_bullet.m_scale + target.m_scale * 0.85f){
                target.m_hit = true;
            }
        }
    }
    
    m_targets.m_targets.remove_if([](const Targets::Target &t) {return t.m_hit;});
}

void OpenGLWindow::checkWinCondition(){
    if(m_targets.m_targets.empty()){
        m_gameData.m_state = State::Win;
        m_restartWaitTimer.restart();
    }
}
