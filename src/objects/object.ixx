export module object;

import std;
import glm;

import math;
import drawable;

export class Object : public Drawable
{
public:
	inline void setModel(const glm::mat4& newModel)
	{
		model = newModel;
	}

	inline const auto& getModel() const
	{
		return model;
	}

	inline const auto& getColor() const
	{
		return color;
	}		

	void translate(const glm::vec3& translation)
	{
		position += translation;
		update();
	}

	void rotate(float angleRadians, const glm::vec3& axis)
	{
		rotation = glm::normalize(glm::angleAxis(angleRadians, axis) * rotation);
		update();
	}

protected:
	glm::mat4 model{ 1.0f };
	glm::vec4 color;
	glm::vec3 position{ 0, 0, 0 };
	glm::quat rotation{ {0,0,0} };

	Object(const glm::vec4& color) : color(color)
	{}

	void update()
	{
		model = math::translate(position) * glm::toMat4(rotation);
	}
};