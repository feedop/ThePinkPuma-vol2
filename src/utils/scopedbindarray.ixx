module;
#include <glad/glad.h>

export module glutils:scopedbindarray;

export struct ScopedBindArray
{
	ScopedBindArray(unsigned int VAO)
	{
		glBindVertexArray(VAO);
	}

	~ScopedBindArray()
	{
		glBindVertexArray(0);
	}
};