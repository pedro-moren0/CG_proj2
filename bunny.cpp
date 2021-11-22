#include "abcg.hpp"
#include "bunny.hpp"

#include <fmt/core.h>
#include <tiny_obj_loader.h>
#include <glm/gtx/hash.hpp>
#include <cppitertools/itertools.hpp>
#include <unordered_map>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>

// Explicit specialization of std::hash for Vertex
namespace std {
    template <>
    struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const noexcept {
            const std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
            return h1;
        }
    };
}  // namespace std

void Bunny::loadModelFromFile(std::string_view path) {
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
        throw abcg::Exception{abcg::Exception::Runtime(
            fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
        throw abcg::Exception{
            abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
    }

    if (!reader.Warning().empty()) {
        fmt::print("Warning: {}\n", reader.Warning());
    }

    const auto& attrib{reader.GetAttrib()};
    const auto& shapes{reader.GetShapes()};

    m_vertices.clear();
    m_indices.clear();

    // A key:value map with key=Vertex and value=index
    std::unordered_map<Vertex, GLuint> hash{};

    // Loop over shapes
    for (const auto& shape : shapes) {
        // Loop over indices
        for (const auto offset : iter::range(shape.mesh.indices.size())) {
            // Access to vertex
            const tinyobj::index_t index{shape.mesh.indices.at(offset)};

            // Vertex position
            const int startIndex{3 * index.vertex_index};
            const float vx{attrib.vertices.at(startIndex + 0)};
            const float vy{attrib.vertices.at(startIndex + 1)};
            const float vz{attrib.vertices.at(startIndex + 2)};

            Vertex vertex{};
            vertex.position = {vx, vy, vz};

            // If hash doesn't contain this vertex
            if (hash.count(vertex) == 0) {
                // Add this index (size of m_vertices)
                hash[vertex] = m_vertices.size();
                // Add this vertex
                m_vertices.push_back(vertex);
            }

            m_indices.push_back(hash[vertex]);
        }
    }
}

void Bunny::initializeGL(GLuint program) {
    m_program = program;

    // Load model
    loadModelFromFile("./assets/bunny.obj");

    // Generate VBO
    abcg::glGenBuffers(1, &m_VBO);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generate EBO
    abcg::glGenBuffers(1, &m_EBO);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices[0]) * m_indices.size(), m_indices.data(),
                     GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create VAO
    abcg::glGenVertexArrays(1, &m_VAO);

    // Bind vertex attributes to current VAO
    abcg::glBindVertexArray(m_VAO);

    abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    const GLint positionAttribute{
        abcg::glGetAttribLocation(m_program, "inPosition")};
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // End of binding to current VAO
    abcg::glBindVertexArray(0);
}

void Bunny::paintGL(Camera camera) {

    m_camera = camera;

    abcg::glUseProgram(m_program);

    const GLint modelMatrixLoc{
        abcg::glGetUniformLocation(m_program, "modelMatrix")};
    const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

    abcg::glBindVertexArray(m_VAO);

    // Draw white bunny
    glm::mat4 model{1.0f};
    model = glm::translate(model, m_startPos);
    model = glm::scale(model, glm::vec3(0.1f));

    if (isPickedUp() && !(debugPickUp)) debugPickUp = isPickedUp();

    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
    abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
    if (!debugPickUp) abcg::glDrawElements(
        GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);


    abcg::glBindVertexArray(0);
    abcg::glUseProgram(0);
}

void Bunny::terminateGL() {

    abcg::glDeleteProgram(m_program);
    abcg::glDeleteBuffers(1, &m_EBO);
    abcg::glDeleteBuffers(1, &m_VBO);
    abcg::glDeleteVertexArrays(1, &m_VAO);
}

bool Bunny::isPickedUp() {
    glm::vec3 playerPosOnPlane {
        glm::vec3(m_camera.m_eye.x, 0, m_camera.m_eye.z)};
    glm::vec3 startPosOnPlane {glm::vec3(m_startPos.x, 0, m_startPos.z)};

    // If camera on same square as bunny
    if (glm::distance(startPosOnPlane, playerPosOnPlane) < 1.0f) return true;
    return false;
}
