#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/vector_angle.hpp>


void OpenGLWindow::handleEvent(SDL_Event& ev) {
    if (ev.type == SDL_KEYDOWN) {
        if (ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w)
            m_dollySpeed = 1.5f;
        if (ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s)
            m_dollySpeed = -1.5f;
        if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a)
            m_truckSpeed = -1.5f;
        if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d)
            m_truckSpeed = 1.5f;
        if (ev.key.keysym.sym == SDLK_o) m_panSpeed = -1.5f;
        if (ev.key.keysym.sym == SDLK_p) m_panSpeed = 1.5f;
    }
    if (ev.type == SDL_KEYUP) {
        if ((ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) &&
            m_dollySpeed > 0)
                m_dollySpeed = 0.0f;
        if ((ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) &&
            m_dollySpeed < 0)
                m_dollySpeed = 0.0f;
        if ((ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) &&
            m_truckSpeed < 0)
                m_truckSpeed = 0.0f;
        if ((ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) &&
            m_truckSpeed > 0)
                m_truckSpeed = 0.0f;
        if (ev.key.keysym.sym == SDLK_o && m_panSpeed < 0) m_panSpeed = 0.0f;
        if (ev.key.keysym.sym == SDLK_p && m_panSpeed > 0) m_panSpeed = 0.0f;
    }
}

void OpenGLWindow::initializeGL() {
    abcg::glClearColor(0, 0, 0, 1);

    // Enable depth buffering
    abcg::glEnable(GL_DEPTH_TEST);

    // Create program
    m_program = createProgramFromFile(getAssetsPath() + "lookat.vert",
                                    getAssetsPath() + "lookat.frag");

    m_ground.initializeGL(m_program);
    m_GC.m_enemy.initializeGL(m_program);
    m_GC.m_bunny.initializeGL(m_program);

    resizeGL(getWindowSettings().width, getWindowSettings().height);
}

void OpenGLWindow::paintGL() {
    update();

    // Clear color buffer and depth buffer
    abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

    abcg::glUseProgram(m_program);

    // Get location of uniform variables (could be precomputed)
    const GLint viewMatrixLoc{
        abcg::glGetUniformLocation(m_program, "viewMatrix")};
    const GLint projMatrixLoc{
        abcg::glGetUniformLocation(m_program, "projMatrix")};

    abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
    abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);


    m_ground.paintGL();
    m_GC.m_bunny.paintGL(m_camera);

    std::vector <glm::vec3> enemyPos {
        glm::vec3( 2, 0,  2),
        glm::vec3(-2, 0,  2),
        glm::vec3( 2, 0, -2),
        glm::vec3(-2, 0, -2),
        glm::vec3( 5, 0,  5),
        glm::vec3(-5, 0, -5),
        glm::vec3( 5, 0, -5),
        glm::vec3(-5, 0,  3),
        glm::vec3(-3, 0,  5)
    };

    std::vector<float> enemyPhases {
        180.0f,
        0.0f,
        0.0f,
        180.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f
    };

    for (const auto i : iter::range(0, (int)enemyPos.size())) {
        m_GC.m_enemy.paintGL(
            m_camera,
            enemyPos.at(i),
            glm::wrapAngle(glm::radians(enemyPhases.at(i)) + m_spinningAngle)
        );
    }

    abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
    abcg::OpenGLWindow::paintUI();

    {
        auto flags {ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse};

        // Game state window
        ImGui::SetNextWindowSize(ImVec2(160, 150));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Player", nullptr, flags);

        ImGui::Text("Detected?: %d", m_GC.m_enemy.detected);
        ImGui::Text("Lives left: %d", m_GC.m_lives);
        if (m_GC.m_bunny.debugPickUp) ImGui::Text("I have pvt. Bunny");
        else ImGui::Text("I don't have pvt. Bunny");
        ImGui::Text("Current position: (%.0f, %.0f)", m_camera.m_eye.x, m_camera.m_eye.z);

        ImGui::End();


        // Instructions window
        ImGui::SetNextWindowSize(ImVec2(750, 150));
        ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - 750, 0));

        ImGui::Begin("Instructions", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::Text("Controles: andar = {w = cima, s = baixo, a = direita, d = esquerda} |");
        ImGui::Text("           rodar camera = {p = horário, o = anti-horário}");
        ImGui::Text("Sua missão é resgatar o soldado Bunny.");
        ImGui::Text("A única coisa que sabemos é que ele está numa das extremidades do tabuleiro.");
        ImGui::Text("Não deixe os inimigos te verem.");
        ImGui::Text("Se eles te verem, você perde uma vida, volta para a posição inicial");
        ImGui::Text("e tem que salvar Bunny novamente.");
        ImGui::Text("Quando um inimigo estiver quase te vendo, ele ficará amarelo,");
        ImGui::Text("e se ele te ver, ficará vermelho.");
        ImGui::Text("Quando os inimigos estão de costas eles ficam acinzentados.");
        ImGui::Text("Para salvar o soldade Bunny, basta caminhar até o quadrado onde ele está.");
        ImGui::Text("Depois disso você deverá retornar para a posição inicial marcada em vermelho.");
        ImGui::Text("Boa sorte!");
        ImGui::End();


        // You lose pop-up
        if (m_GC.m_lose) {
            ImGui::SetNextWindowSize(ImVec2(150, 80));
            ImGui::SetNextWindowPos(
                ImVec2((m_viewportWidth - 150) / 2, (m_viewportHeight - 80) / 2));

            ImGui::Begin("You lost!", nullptr, flags);
            ImGui::Text("Want to try again?");
            if (ImGui::Button("Restart")) {
                m_GC.m_lose = false;
                m_GC.m_lives = 3;
            }
            ImGui::End();
        }


        // You win pop-up
        if (m_GC.m_win) {
            ImGui::SetNextWindowSize(ImVec2(150, 80));
            ImGui::SetNextWindowPos(
                ImVec2((m_viewportWidth - 150) / 2, (m_viewportHeight - 80) / 2));

            ImGui::Begin("You won!", nullptr, flags);
            ImGui::Text("Want to play again?");
            if (ImGui::Button("Restart")) {
                m_GC.m_win = false;
                // because of a little bug i can't fix
                m_GC.m_lives = 4;
                m_GC.m_bunny.debugPickUp = false;
            }
            ImGui::End();
        }
    }
}

void OpenGLWindow::resizeGL(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;

    m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
    m_ground.terminateGL();
    m_GC.m_enemy.terminateGL();
    m_GC.m_bunny.terminateGL();

    abcg::glDeleteProgram(m_program);
}

void OpenGLWindow::update() {
    const float deltaTime{static_cast<float>(getDeltaTime())};

    // Freeze scene and lock controls if lost or won
    if (!(m_GC.m_lose || m_GC.m_win)) {
        if (m_GC.m_enemy.detected) {
            --m_GC.m_lives;
            m_timer.restart();
            while (m_timer.elapsed() <= 750 / 1000.0) {}
            m_GC.m_bunny.debugPickUp = false;
            m_GC.m_enemy.detected = false;
            m_camera.m_eye = m_GC.m_playerStartPos;
            m_camera.m_at = m_GC.m_playerStartLook;
            m_camera.computeViewMatrix();
            if (m_GC.m_lives <= 0) m_GC.m_lose = true;
        }

        m_GC.checkWinCondition(m_camera.m_eye);
        if (m_GC.m_win) {
            m_camera.m_eye = m_GC.m_playerStartPos;
            m_camera.m_at = m_GC.m_playerStartLook;
            m_camera.computeViewMatrix();
        }

        // Update LookAt camera
        m_camera.dolly(m_dollySpeed * deltaTime);
        m_camera.pan(m_panSpeed * deltaTime);
        m_camera.truck(m_truckSpeed * deltaTime);
        m_spinningAngle = glm::wrapAngle(m_spinningAngle + deltaTime * glm::radians(15.0f));
    }
}
