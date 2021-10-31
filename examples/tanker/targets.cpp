#include "targets.hpp"
#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Targets::initializeGL(GLuint program, int qtd){
    terminateGL();

    m_randomEngine.seed(std::chrono::steady_clock::now().time_since_epoch().count());

    m_program = program;
    m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
    m_scaleLoc = abcg::glGetUniformLocation(m_program, "rotation");
    m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

    m_targets.clear();
    m_targets.resize(qtd);

    for(auto &target : m_targets){
        target = createTarget();
        target.m_translation = {m_randomDist(m_randomEngine), m_randomDist(m_randomEngine)};
    }
}

void Targets::paintGL(){
    abcg::glUseProgram(m_program);

    for (const auto &target : m_targets){
        abcg::glBindVertexArray(target.m_vao);

        abcg::glUniform4fv(m_colorLoc, 1, &target.m_color.r);
        abcg::glUniform1f(m_scaleLoc, target.m_scale);
        
        //for(const auto &target : m_targets){
        //    abcg::glUniform2f(m_translationLoc, target.m_translation.x, target.m_translation.y);
            abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, target.m_polygonSides+2);
        //}

        abcg::glBindVertexArray(0);


    }
    abcg::glUseProgram(0);
}

void Targets::terminateGL(){
    for(auto target : m_targets){
        abcg::glDeleteBuffers(1,&target.m_vbo);
        abcg::glDeleteVertexArrays(1,&target.m_vao);
    }
}

void Targets::update(float deltaTime){
    for(auto &target : m_targets){
        target.m_translation += sin(deltaTime);
    }
}

Targets::Target Targets::createTarget(glm::vec2 translation){

    Target target;

    target.m_polygonSides = 50;
    target.m_color = glm::vec4(0.8f,0.33f,0,1);
    target.m_scale = 0.2f;


    target.m_translation = translation;

    std::vector<glm::vec2> positions(0);
    positions.emplace_back(0,0);
    const auto step{M_PI *2 / target.m_polygonSides};
    for(const auto angle : iter::range(0.0,M_PI*2,step)){
        const auto radius{1.0f};
        positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
    }
    positions.push_back(positions.at(1));

    abcg::glGenBuffers(1, &target.m_vbo);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, target.m_vbo);
    abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

    abcg::glGenVertexArrays(1,&target.m_vao);

    abcg::glBindVertexArray(target.m_vao);

    abcg::glBindBuffer(GL_ARRAY_BUFFER, target.m_vbo);
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    abcg::glBindBuffer(GL_ARRAY_BUFFER,0);

    abcg::glBindVertexArray(0);

    return target;


}