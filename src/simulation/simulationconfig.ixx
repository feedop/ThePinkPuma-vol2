export module simulation:config;

import std;
import glm;

import math;

export struct SimulationConfig
{
	float dt;
	float totalTime;
	glm::vec3 startPosition;
	glm::vec3 endPosition;
	glm::quat startQuat;
	glm::quat endQuat;
};