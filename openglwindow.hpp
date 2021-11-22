#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <vector>

#include "abcg.hpp"
#include "camera.hpp"
#include "ground.hpp"
#include "enemy.hpp"
#include "bunny.hpp"
#include "game_controler.hpp"


class OpenGLWindow : public abcg::OpenGLWindow {
    protected:
        void handleEvent(SDL_Event& ev) override;
        void initializeGL() override;
        void paintGL() override;
        void paintUI() override;
        void resizeGL(int width, int height) override;
        void terminateGL() override;

    private:
        GLuint m_program{};

        int m_viewportWidth{};
        int m_viewportHeight{};

        Camera m_camera;
        float m_dollySpeed{0.0f};
        float m_truckSpeed{0.0f};
        float m_panSpeed{0.0f};

        float m_spinningAngle{};

        Ground m_ground;
        GameControler m_GC;

        abcg::ElapsedTimer m_timer;

        void update();
};

#endif
