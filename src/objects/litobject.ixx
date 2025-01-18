module;
#include <glad/glad.h>

export module litobject;

import std;
import glm;

import object;
import glutils;
import shader;

export struct VertexWithNormal
{
	glm::vec3 position;
	glm::vec3 normal;
};

export class LitObject : public Object
{
public:
	virtual void draw(const Shader* shader) const
	{
		ScopedBindArray ba(VAO);
		shader->setMatrix("model", model);
		shader->setVector("color", color);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
	}

protected:
	unsigned int vertexCount;
	unsigned int indexCount;

	LitObject(const std::vector<VertexWithNormal>& vertices, const std::vector<unsigned int>& indices, const glm::vec4& color) : Object(color), vertexCount(vertices.size()), indexCount(indices.size())
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexWithNormal), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWithNormal), (void*)0);
		// normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWithNormal), (void*)sizeof(glm::vec3));
	}
};