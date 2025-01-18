module;
#include <glad/glad.h>

export module grid;

import std;
import glm;

import drawable;
import math;
import glutils;
import quad;
import shader;

export class Grid : public Drawable
{
public:
	Grid()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexWithTex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
					 &indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWithTex), (void*)0);
		// texture coordinates
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWithTex), (void*)sizeof(glm::vec3));
	}

	virtual void draw(const Shader * shader) const override
	{
		ScopedBindArray ba(VAO);
		shader->setMatrix("model", model);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	}

private:
	inline static const std::vector<VertexWithTex> vertices = Quad::createVerticesWithTex();
	inline static const std::vector<unsigned int> indices = Quad::createIndices();

	glm::mat4 model = math::scale(10.0f) * math::rotate(-math::pi / 2, { 1.0f, 0.0, 0.0 });;
};