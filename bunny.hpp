#ifndef BUNNY_HPP_
#define BUNNY_HPP_

#include <vector>

#include "abcg.hpp"
#include "camera.hpp"

struct Vertex {
    glm::vec3 position;

    bool operator==(const Vertex& other) const {
        return position == other.position;
    }
};

class Bunny {
    public:
        void initializeGL(GLuint program);
        void paintGL(Camera camera);
        void terminateGL();
        bool isPickedUp();

        bool debugPickUp {false};

    private:
        GLuint m_VAO{};
        GLuint m_VBO{};
        GLuint m_EBO{};
        GLuint m_program{};

        glm::vec3 m_startPos{glm::vec3(-5.0f, 0.0f, 5.0f)};

        std::vector<Vertex> m_vertices;
        std::vector<GLuint> m_indices;

        Camera m_camera{};

        void loadModelFromFile(std::string_view path);
};

#endif
