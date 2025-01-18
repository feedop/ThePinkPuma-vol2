module;
#include <glad/glad.h>

export module glutils:scopedlinewidth;

export struct ScopedLineWidth
{
	ScopedLineWidth(float width)
	{
		glLineWidth(width);
	}

	~ScopedLineWidth()
	{
		glLineWidth(1.0f);
	}
};