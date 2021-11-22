#ifndef ENEMY_HPP_
#define ENEMY_HPP_

#include "abcg.hpp"
#include "camera.hpp"

#include <vector>

class GameControler;

class Enemy {
    public:
        void initializeGL(GLuint program);
        void paintGL(Camera camera, glm::vec3 position, float phase);
        void terminateGL();
        bool isDetected(
            glm::mat4 model,
            glm::vec3 refPoint,
            float width,
            float distance,
            float phase
        );

        glm::vec3 vectorDebug{};
        bool detected{false};
    private:
        friend GameControler;

        GLuint m_program{};
        GLuint m_vbo{};
        GLuint m_vao{};

        GLint m_modelMatrixLoc{};
        GLint m_colorLoc{};

        Camera m_camera{};

};

#endif
