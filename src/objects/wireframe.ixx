module;
#include <glad/glad.h>

export module wireframe;

import std;
import glm;

import puma;
import glutils;
import shader;
import colors;

// Function to extract position from model matrix
glm::vec3 getPositionFromModelMatrix(const glm::mat4& modelMatrix)
{
    return glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
}

// Function to generate wireframe cylinder
std::vector<glm::vec3> generateWireframe(const glm::vec3& start, const glm::vec3& end, int segments = 4)
{
    std::vector<glm::vec3> vertices;

    // Calculate direction and perpendicular vector
    glm::vec3 direction = glm::normalize(end - start);
    glm::vec3 up = glm::vec3(0, 1, 0);
    if (glm::abs(glm::dot(direction, up)) > 0.99f) // Avoid parallel vectors
        up = glm::vec3(1, 0, 0);
    glm::vec3 right = glm::normalize(glm::cross(direction, up));
    up = glm::normalize(glm::cross(right, direction));

    float radius = 0.1f; // Cylinder radius
    float angleStep = 2.0f * glm::pi<float>() / segments;

    // Generate circle points at both ends
    for (int i = 0; i < segments; ++i)
    {
        float angle = i * angleStep;
        glm::vec3 offset = right * glm::cos(angle) * radius + up * glm::sin(angle) * radius;
        vertices.push_back(start + offset);
        vertices.push_back(end + offset);
    }

    return vertices;
}

export class Wireframe : public Puma
{
public:
    Wireframe()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        ScopedBindArray ba(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
    }

    virtual void setData(const std::array<glm::mat4, 6>& modelMatrices) override
    {
        std::vector<glm::vec3> positions;
        for (const auto& mat : modelMatrices)
        {
            positions.push_back(getPositionFromModelMatrix(mat));
        }

        // Generate wireframe cylinders
        std::vector<glm::vec3> wireframeVertices;
        for (int i = 0; i < positions.size() - 1; ++i)
        {
            auto segmentVertices = generateWireframe(positions[i], positions[i + 1]);
            wireframeVertices.insert(wireframeVertices.end(), segmentVertices.begin(), segmentVertices.end());
        }

        ScopedBindArray ba(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, wireframeVertices.size() * sizeof(glm::vec3), wireframeVertices.data(), GL_DYNAMIC_DRAW);
        vertexCount = wireframeVertices.size();
    }

    virtual void draw(const Shader* shader) const
    {
        ScopedBindArray ba(VAO);
        shader->setVector("color", color);
        shader->setMatrix("model", model);
        glDrawArrays(GL_LINES, 0, vertexCount);
    }

private:
    int vertexCount = 0;
};