export module scene;

import std;
import glm;

import object;
import marker;
import grid;
import drawable;
import colors;
import wireframe;
import cursor;
import puma;
import solidpuma;

export class Scene
{
public:
	Scene()
	{
		for (int i = 0; i < 6; i++)
		{
			auto marker = std::make_unique<Marker>(colors::markerColors[i]);
			markers.push_back(std::move(marker));
		}
				
		wireframe = std::make_unique<Wireframe>();
		solidPuma = std::make_unique<SolidPuma>();
	}


	inline auto& getMarkers() const
	{
		return markers;
	}

	inline auto& getWireframe() const
	{
		return wireframe;
	}

	inline auto& getSolidPuma() const
	{
		return solidPuma;
	}

	void setModels(const std::array<glm::mat4, 6>& models)
	{
		for (int i = 0; i < 6; i++)
		{
			markers[i]->setModel(models[i]);
		}

		markers[1]->setCursorModel(models[0]);
		markers[3]->setCursorModel(models[1]);
		markers[4]->setCursorModel(models[3]);
		markers[5]->setCursorModel(models[4]);

		wireframe->setData(models);
		solidPuma->setData(models);
	}

private:
	std::vector<std::unique_ptr<Marker>> markers;
	std::unique_ptr<Puma> wireframe;
	std::unique_ptr<Puma> solidPuma;
};