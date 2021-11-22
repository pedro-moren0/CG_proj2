#include "abcg.hpp"
#include "openglwindow.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <vector>

void Enemy::initializeGL(GLuint program) {
    terminateGL();

    m_program = program;

    // Create trial triangle
    std::array vertices{glm::vec3( 0.0f, 0.0f,  0.5f),
                        glm::vec3(-0.5f, 0.0f, -0.5f),
                        glm::vec3( 0.5f, 0.0f, -0.5f)};

    // Generate triangle VBO
    abcg::glGenBuffers(1, &m_vbo);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
                       GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Get position attribute
    GLint positionAttribute{
        abcg::glGetAttribLocation(m_program, "inPosition")
    };

    // Create tri VAO
    abcg::glGenVertexArrays(1, &m_vao);

    // Bind vertex attributes to current VAO
    abcg::glBindVertexArray(m_vao);

    abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vao);
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glBindVertexArray(0);

    // Save location of uniform variables
    m_modelMatrixLoc = abcg::glGetUniformLocation(m_program, "modelMatrix");
    m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
}

void Enemy::paintGL(Camera camera, glm::vec3 position, float phase) {

    m_camera = camera;

    abcg::glUseProgram(m_program);
    abcg::glBindVertexArray(m_vao);

    // Initializes color with white
    glm::vec4 color {glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)};

    glm::mat4 model {glm::mat4(1.0)};
    // Translate each enemy to its position + up
    model = glm::translate(model, position + glm::vec3(0.0f, 0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
    glm::vec3 refPoint {glm::vec3(0.0f, 0.0f, 0.5f)};

    // pass phase as an argument
    if (isDetected(model, refPoint, 90.0f, 3.0f, phase) ||
        isDetected(model, refPoint, 360.f, 1.5f, phase)) {
        // Changes color to yellow
        color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    }
    if (isDetected(model, refPoint, 60.0f, 2.5f, phase)) {
        // Changes color to red
        color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        detected = true;
    }
    // Have to do this here to wont interfere with model in isDetected
    // Turn around k because of last rotation
    model = glm::rotate(model, phase, glm::vec3(0, 0, 1));

    abcg::glUniform4f(m_colorLoc, color.r, color.g, color.b, color.a);

    abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
    abcg::glDrawArrays(GL_TRIANGLES, 0, 3);

    abcg::glBindVertexArray(0);
}

void Enemy::terminateGL() {
    abcg::glDeleteProgram(m_program);
    abcg::glDeleteVertexArrays(1, &m_vao);
    abcg::glDeleteBuffers(1, &m_vbo);
}

bool Enemy::isDetected(
    glm::mat4 model, glm::vec3 refPoint, float width, float distance, float phase)
{
    //receives phase as parameter
    glm::vec4 refPointOnPlane {glm::vec4(refPoint, 1.0f)};

    // Cuidado com essa multipl. com model
    glm::vec3 transformedRefPoint {glm::vec3(
        (model * refPointOnPlane).x,
        0.0f,
        (model * refPointOnPlane).z
    )};

    glm::vec3 eyePosOnPlane {glm::vec3(m_camera.m_eye.x, 0, m_camera.m_eye.z)};

    glm::vec3 refEyeVec {transformedRefPoint - eyePosOnPlane};


    float visionFieldWidth {glm::radians(width)};
    glm::vec3 visionFieldVec {glm::vec3(0, 0, 1)};
    // Lembra daquele bug que deu
    visionFieldVec = glm::rotate(visionFieldVec, -phase, glm::vec3(0, 1, 0));

    float currentAngle {glm::acos(glm::dot(
            glm::normalize(refEyeVec),
            glm::normalize(visionFieldVec)
    ))};

    if (currentAngle < (visionFieldWidth / 2.0f)
        && glm::distance(transformedRefPoint, eyePosOnPlane) < distance) {
            return true;
    }

    return false;
}
