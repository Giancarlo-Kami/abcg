#include "tank.hpp"
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Tank::initializeGL(GLuint program){
    terminateGL();

    m_program = program;
    m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
    m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
    m_angleLoc = abcg::glGetUniformLocation(m_program, "angle");


    std::array<glm::vec2, 18> positions{

        glm::vec2{-10.0f,+00.0f}, glm::vec2{-06.0f,+00.0f}, glm::vec2{-06.0f,-05.0f},
        glm::vec2{+06.0f,+00.0f}, glm::vec2{+06.0f,-05.0f}, glm::vec2{+10.0f,+00.0f},

        glm::vec2{-03.0f,+01.0f}, glm::vec2{+03.0f,+01.0f}, glm::vec2{-03.0f,+00.0f},
        glm::vec2{+03.0f,+00.0f},

        glm::vec2{-05.0f,+05.0f}, glm::vec2{-05.0f,+01.0f}, glm::vec2{+05.0f,+05.0f},
        glm::vec2{+05.0f,+01.0f},

        glm::vec2{+04.5f,+03.0f}, glm::vec2{+04.5f,+02.0f}, glm::vec2{+7.5f,+03.0f},
        glm::vec2{+07.5f,+02.0f}
    };

    for(auto &position : positions){
        position /= glm::vec2{10.0f,05.0f};
        position -= glm::vec2{5.8f,5.0f};
    }

    const std::array indices{0,1,2,
                             1,2,4,
                             1,3,4,
                             3,4,5,
                             6,7,9,
                             6,8,9,
                             10,11,13,
                             10,12,13,
                             14,15,16,
                             15,16,17};
    
    abcg::glGenBuffers(1,&m_vbo);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glGenBuffers(1,&m_ebo);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLint positionAttribute{abcg::glGetAttribLocation(m_program,"inPosition")};

    abcg::glGenVertexArrays(1,&m_vao);

    abcg::glBindVertexArray(m_vao);

    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    abcg::glBindVertexArray(0);
}

void Tank::paintGL(const GameData &gameData){
    if(gameData.m_state != State::Playing) return;

    abcg::glUseProgram(m_program);

    abcg::glBindVertexArray(m_vao);

    abcg::glUniform1f(m_scaleLoc, m_scale);

    abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
    abcg::glDrawElements(GL_TRIANGLES, 18*3, GL_UNSIGNED_INT, nullptr);
    abcg::glBindVertexArray(0);
    abcg::glUseProgram(0);
}

void Tank::terminateGL(){
    abcg::glDeleteBuffers(1,&m_vbo);
    abcg::glDeleteBuffers(1,&m_ebo);
    abcg::glDeleteVertexArrays(1,&m_vao);
}
