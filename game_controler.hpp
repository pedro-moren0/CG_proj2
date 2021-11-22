#ifndef GAME_CONTROLER_HPP_
#define GAME_CONTROLER_HPP_

#include "enemy.hpp"
#include "bunny.hpp"

class OpenGLWindow;

class GameControler {
    public:
        void checkWinCondition(glm::vec3 currentPosition) {
            if (m_bunny.debugPickUp
                && glm::distance(currentPosition, glm::vec3(0)) < 1.0f)
                m_win = true;
            else m_win = false;
        };

    private:
        friend OpenGLWindow;

        Enemy m_enemy;
        Bunny m_bunny;

        int m_lives{3};

        bool m_lose{false};
        bool m_win{false};

        glm::vec3 m_playerStartPos {glm::vec3(0.0f, 0.5f, 0.001f)};
        glm::vec3 m_playerStartLook {glm::vec3(0.0f, 0.5f, 0.0f)};

};

#endif
