#include "bullet.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Bullets::initializeGL(GLuint program){
    terminateGL();

    m_program = program;
    m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
    m_scaleLoc = glGetUniformLocation(m_program, "scale");
    m_translationLoc = glGetUniformLocation(m_program, "translation");

    m_bullet.clear();

    const auto sides{10};

    std::vector<glm::vec2> positions(0);
    positions.emplace_back(0,0);
    const auto step{M_PI*2/sides};
    for(const auto angle : iter::range(0.0,M_PI * 2, step)){
        positions.emplace_back(std::cos(angle),std::sin(angle));
    }
    positions.push_back(positions.at(1));

    abcg::glGenBuffers(1,&m_vbo);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    abcg::glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(glm::vec2), positions.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    const GLint positionAttribute{
        abcg::glGetAttribLocation(m_program, "inPosition")};
    abcg::glGenVertexArrays(1,&m_vao);
    abcg::glBindVertexArray(m_vao);

    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glBindVertexArray(0);

}

void Bullets::paintGL(){
    abcg::glUseProgram(m_program);

    abcg::glBindVertexArray(m_vao);
    abcg::glUniform4f(m_colorLoc, 1,1,1,1);
    abcg::glUniform1f(m_scaleLoc, m_scale);

    for(const auto&bullet : m_bullet){
        abcg::glUniform2f(m_translationLoc, bullet.m_translation.x, bullet.m_translation.y);
        abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
    }

    abcg::glBindVertexArray(0);

    abcg::glUseProgram(0);
}

void Bullets::terminateGL(){
    abcg::glDeleteBuffers(1,&m_vbo);
    abcg::glDeleteVertexArrays(1,&m_vao);
}

void Bullets::update(Tank &tank, const GameData &gameData, float DeltaTime){
    if(gameData.m_input[static_cast<size_t>(Input::Fire) && gameData.m_state == State::Playing]){
        if(tank.m_bulletCoolDownTimer.elapsed() > 1){
            tank.m_bulletCoolDownTimer.restart();

            Bullet bullet{.m_dead = false, .m_translation = glm::vec2{-1.0f,-1.0f}, .m_velocity = glm::vec2{0.5f,0.5f} * tank.m_angle};
            m_bullet.push_back(bullet);
        }
    }

    for(auto &bullet : m_bullet){
        bullet.m_translation +=bullet.m_velocity * DeltaTime;
        bullet.m_translation -=glm::vec2{0,-0.2f} * DeltaTime;

        if(bullet.m_translation.x < -1.1f) bullet.m_dead = true;
        if(bullet.m_translation.x > +1.1f) bullet.m_dead = true;
        if(bullet.m_translation.y < -1.1f) bullet.m_dead = true;
        if(bullet.m_translation.y > +1.1f) bullet.m_dead = true;
    }

    m_bullet.remove_if([](const Bullet &p) {return p.m_dead;});
}
