export module solidpuma;

import std;
import glm;

import puma;
import glutils;
import shader;
import colors;
import cylinder;
import math;

// Function to compute model matrix for a cylinder between two joints
glm::mat4 computeCylinderModelMatrix(const glm::vec3& joint1, const glm::vec3& joint2, float baseScale = 0.3f)
{
    // Calculate the midpoint
    glm::vec3 midpoint = (joint1 + joint2) * 0.5f;

    // Calculate the direction and length
    glm::vec3 direction = joint2 - joint1;
    float length = glm::length(direction);
    glm::vec3 normalizedDirection = glm::normalize(direction);

    // Create rotation quaternion to align cylinder's local Y-axis with the direction vector
    static const glm::vec3 cylinderUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::quat rotation = glm::rotation(cylinderUp, normalizedDirection);

    // Construct the model matrix
    glm::mat4 modelMatrix = math::translate(midpoint) * glm::mat4_cast(rotation) * math::scale(glm::vec3(baseScale, length / 2 + 0.2f, baseScale)); // Scale cylinder

    return modelMatrix;
}

export class SolidPuma : public Puma
{
public:
    SolidPuma()
    {}

    virtual void setData(const std::array<glm::mat4, 6>& jointMatrices) override
    {
        std::vector<glm::vec3> jointPositions;
        for (const auto& mat : jointMatrices)
        {
            jointPositions.push_back(glm::vec3(mat[3][0], mat[3][1], mat[3][2]));
        }

        // Compute model matrices for cylinders
        parts[0].setModel(computeCylinderModelMatrix(jointPositions[0], jointPositions[1]));
        parts[1].setModel(computeCylinderModelMatrix(jointPositions[1], jointPositions[3]));
        parts[2].setModel(computeCylinderModelMatrix(jointPositions[3], jointPositions[4]));
        parts[3].setModel(computeCylinderModelMatrix(jointPositions[4], jointPositions[5]));
    }

    virtual void draw(const Shader* shader) const override
    {
        for (auto& part : parts)
        {
            shader->setVector("color", color);
            shader->setMatrix("model", part.getModel());
            part.draw(shader);
        }
    }

private:
    int vertexCount = 0;
    std::array<Cylinder, 4> parts
    {
        color,
        color,
        color,
        color
    };
};