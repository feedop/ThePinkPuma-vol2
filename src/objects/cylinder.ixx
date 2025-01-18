module;
#include <glad/glad.h>
export module cylinder;

import std;
import glm;

import colors;
import litobject;
import math;
import glutils;
import shader;

std::vector<VertexWithNormal> createVerticesWithNormal(float radius = 0.5f, float height = 2.0f, int radialSegments = 36)
{
    std::vector<VertexWithNormal> vertices;
    float angleStep = 2.0f * math::pi / radialSegments;
    float halfHeight = height / 2.0f;

    // Generate side vertices and normals
    for (int i = 0; i <= radialSegments; ++i)
    {
        float angle = i * angleStep;
        float x = radius * glm::cos(angle);
        float z = radius * glm::sin(angle);
        glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));

        // Top and bottom vertices of the side
        vertices.push_back({ glm::vec3(x, halfHeight, z), normal });   // Top vertex
        vertices.push_back({ glm::vec3(x, -halfHeight, z), normal }); // Bottom vertex
    }

    // Generate top cap vertices
    for (int i = 0; i <= radialSegments; ++i)
    {
        float angle = i * angleStep;
        float x = radius * glm::cos(angle);
        float z = radius * glm::sin(angle);

        vertices.push_back({ glm::vec3(x, halfHeight, z), glm::vec3(0.0f, 1.0f, 0.0f) });
    }
    // Add center vertex for the top cap
    vertices.push_back({ glm::vec3(0.0f, halfHeight, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) });

    // Generate bottom cap vertices
    for (int i = 0; i <= radialSegments; ++i)
    {
        float angle = i * angleStep;
        float x = radius * glm::cos(angle);
        float z = radius * glm::sin(angle);

        vertices.push_back({ glm::vec3(x, -halfHeight, z), glm::vec3(0.0f, -1.0f, 0.0f) });
    }
    // Add center vertex for the bottom cap
    vertices.push_back({ glm::vec3(0.0f, -halfHeight, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) });

    return vertices;
}

std::vector<unsigned int> createIndices(int radialSegments = 36)
{
    std::vector<unsigned int> indices;

    // Side indices
    for (int i = 0; i < radialSegments; ++i)
    {
        int top1 = i * 2;
        int bottom1 = top1 + 1;
        int top2 = top1 + 2;
        int bottom2 = bottom1 + 2;

        // Triangle 1
        indices.push_back(top1);
        indices.push_back(bottom1);
        indices.push_back(top2);

        // Triangle 2
        indices.push_back(bottom1);
        indices.push_back(bottom2);
        indices.push_back(top2);
    }

    // Top cap indices
    int topStart = (radialSegments + 1) * 2;
    int topCenter = topStart + radialSegments + 1;
    for (int i = 0; i < radialSegments; ++i)
    {
        indices.push_back(topCenter);
        indices.push_back(topStart + i);
        indices.push_back(topStart + i + 1);
    }

    // Bottom cap indices
    int bottomStart = topCenter + 1;
    int bottomCenter = bottomStart + radialSegments + 1;
    for (int i = 0; i < radialSegments; ++i)
    {
        indices.push_back(bottomCenter);
        indices.push_back(bottomStart + i + 1);
        indices.push_back(bottomStart + i);
    }

    return indices;
}


export class Cylinder : public LitObject
{
public:
    Cylinder(const glm::vec4& color) : LitObject(createVerticesWithNormal(), createIndices(), color)
	{
        model = glm::mat4{ 1.0f };
	}

	virtual void draw(const Shader* shader) const
	{
		ScopedBindArray ba(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
	}
};