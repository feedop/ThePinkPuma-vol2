export module simulation:data;

import std;
import glm;

export struct SimulationData
{
	double t;
	std::array<float, 6> q1;
	std::array<float, 6> q2;
};