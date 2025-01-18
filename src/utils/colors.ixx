export module colors;

import std;
import glm;

export namespace colors
{
	inline const glm::vec4 transparent = { 0.0f, 0.0f, 0.0f, 0.0f };
	
	inline const glm::vec4 white = { 1.0f, 1.0f, 0.8f, 1.0f };

	inline const glm::vec4 yellow = { 1.0f, 1.0f, 0.0f, 1.0f };

	inline const glm::vec4 orange = { 1.0f, 0.6f, 0.0f, 1.0f };

	inline const glm::vec4 cyan = { 0.0f, 0.8f, 0.9f, 1.0f };

	inline const glm::vec4 red =  { 1.0f, 0.0f, 0.0f, 1.0f };

	inline const glm::vec4 green = { 0.0f, 1.0f, 0.0f, 1.0f };

	inline const glm::vec4 blue = { 0.0f, 0.0f, 1.0f, 1.0f };

	inline const glm::vec4 navy = { 0.0f, 0.3f, 0.5f, 1.0f };

	inline const glm::vec4 pink = { 1.0f, 0.2f, 0.7f, 1.0f };

	inline const glm::vec4 transparentPink = { 1.0f, 0.6f, 1.0f, 0.6f };

	inline const glm::vec4 transparentCyan = { 0.0f, 0.6f, 0.7f, 0.6f };

	inline const std::array<glm::vec4, 6> markerColors
	{
		white, yellow, pink, orange, cyan, navy
	};
}