module;
#include <glad/glad.h>

export module cursor;

import std;
import glm;

import colors;
import unlitobject;
import glutils;
import shader;
import math;

std::vector<VertexWithColor> createVertices()
{
	return {
		{
			{0.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f}
		},
		{
			{1.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f}
		},
		{
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}
		},
		{
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}
		},
		{
			{0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f}
		},
		{
			{0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 0.0f}
		}
	};
}

// Generates indices for rendering the cube using triangles
std::vector<unsigned int> createIndices()
{
    return {
        0, 1,
        2, 3,
        4, 5
    };
}

export class Cursor : public UnlitObject
{
public:
    Cursor(float scale = 1.0f) : UnlitObject(createVertices(), createIndices(), colors::green)
    {
		model = glm::mat4{ scale };
	}

    virtual void draw(const Shader* shader) const override
    {
        ScopedBindArray ba(VAO);
        shader->setMatrix("model", model);
        glDrawElements(GL_LINES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
    }
};