module;
#include <glad/glad.h>

export module renderer;

import std;
import glm;

import camera;
import colors;
import config;
import drawable;
import shader;
import simulation;
import grid;
import object;
import litobject;
import math;
import scene;
import cursor;
import inversekinematics;

class GuiController;

/// <summary>
/// Responsible for rendering the scene using OpenGL
/// </summary>
export class Renderer
{
public:
	Renderer(int windowWidth, int windowHeight, Camera& camera, SimulationEngine& simulation, Scene& scene1, Scene& scene2) :
		windowWidth(windowWidth), windowHeight(windowHeight), camera(camera), simulation(simulation), scene1(scene1), scene2(scene2)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_PROGRAM_POINT_SIZE);

		grid = std::make_unique<Grid>();
		cursor = std::make_unique<Cursor>();
	}

	/// <summary>
	/// Render the scene
	/// </summary>
	void draw(const SimulationData& data)
	{
		// Clear
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0.15f, 0.0f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (dataChanged || simulation.isRunning())
		{
			setModels(data, scene1, data.q1);
			setModels(data, scene2, data.q2);
			dataChanged = false;
		}

		if (splitScreen)
		{
			glViewport(0, 0, windowWidth / 2, windowHeight);
			drawScene(data, scene1);
			glViewport(windowWidth / 2, 0, windowWidth / 2, windowHeight);
			drawScene(data, scene2);
		}
		else
		{
			glViewport(0, 0, windowWidth, windowHeight);
			drawScene(data, scene1);
		}
	}

	void setWindowSize(int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
	}

	inline void signalDataChange()
	{
		dataChanged = true;
	}

	void rotateCursor(float xDiff, float yDiff)
	{
		float angleRadians = std::sqrtf(xDiff * xDiff + yDiff * yDiff);
		glm::vec3 axis = glm::normalize(invRotateZUp * glm::vec4((camera.getUp() * xDiff + camera.getRight() * yDiff), 0.0f));
		cursor->rotate(angleRadians, axis);
		updateFromCursor();
	}

	void moveCursor(float xDiff, float yDiff)
	{
		cursor->translate(getTranslationFromMouse(xDiff, yDiff));
		updateFromCursor();
	}

	inline const glm::mat4& getCursorModel() const
	{
		return cursor->getModel();
	}

	inline void passLToSimulation()
	{
		simulation.setL(l1, l3, l4);
	}

public: // Friend not working, probably because of a bug in Clang
	friend class GuiController;

	Camera& camera;
	SimulationEngine& simulation;
	Scene& scene1;
	Scene& scene2;

	std::unique_ptr<Shader> uniformColorShader = std::make_unique<UniformColorShader>();
	std::unique_ptr<Shader> phongShader = std::make_unique<PhongShader>();
	std::unique_ptr<Shader> gridShader = std::make_unique<InfiniteGridShader>();
	std::unique_ptr<Shader> multiColorShader = std::make_unique<MultiColorShader>();

	std::unique_ptr<Drawable> grid;
	std::unique_ptr<Object> cursor;

	int windowWidth;
	int windowHeight;
	bool drawGrid = true;
	bool splitScreen = true;
	bool followCursor = false;
	bool shadedPuma = false;
	bool markersOnTop = true;
	bool renderExtensionMarker = true;
	bool dataChanged = true;

	float l1 = 1.0f;
	float l3 = 1.0f;
	float l4 = 1.0f;

	inline static const glm::mat4 invRotateZUp = glm::inverse(glm::mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, -1.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    ));

	void drawScene(const SimulationData& data, const Scene& scene)
	{
		drawObjects(data, scene);

		// Grid	
		if (drawGrid)
		{
			gridShader->use();
			gridShader->setMatrix("view", camera.getView());
			gridShader->setMatrix("projection", camera.getProjection());
			grid->draw(gridShader.get());
		}
	}

	void drawObjects(const SimulationData& data, const Scene& scene) const
	{
		multiColorShader->use();
		multiColorShader->setMatrix("view", camera.getView());
		multiColorShader->setMatrix("projection", camera.getProjection());
		cursor->draw(multiColorShader.get());

		auto& markers = scene.getMarkers();
		for (auto& marker : markers)
		{
			marker->drawCursor(multiColorShader.get());
		}		

		if (shadedPuma)
		{
			phongShader->use();
			phongShader->setMatrix("view", camera.getView());
			phongShader->setMatrix("projection", camera.getProjection());
			scene.getSolidPuma()->draw(phongShader.get());
		}
		else
		{
			uniformColorShader->use();
			uniformColorShader->setMatrix("view", camera.getView());
			uniformColorShader->setMatrix("projection", camera.getProjection());
			scene.getWireframe()->draw(uniformColorShader.get());

			phongShader->use();
			phongShader->setMatrix("view", camera.getView());
			phongShader->setMatrix("projection", camera.getProjection());
		}

		if (markersOnTop)
			glDisable(GL_DEPTH_TEST);
		for (int i = 0; i < markers.size(); i++)
		{
			if (!renderExtensionMarker && i == 2)
				continue;
			markers[i]->draw(phongShader.get());
		}
		if (markersOnTop)
			glEnable(GL_DEPTH_TEST);
	}

	void setModels(const SimulationData& data, Scene& scene, const std::array<float, 6>& q)
	{
		std::array<glm::mat4, 6> f;
		f[0] = math::rotate(q[0], { 0,0,1 });
		f[1] = f[0] * math::translate( { 0,0,l1 }) * math::rotate(q[1], { 0,1,0 });
		f[2] = f[1] * math::translate( { q[2] / 2,0,0 });
		f[3] = f[2] * math::translate( { q[2] / 2,0,0 }) * math::rotate(q[3], { 0,1,0 });
		f[4] = f[3] * math::translate( { 0,0,l3 }) * math::rotate(q[4], { 0,0,1 });
		f[5] = f[4] * math::translate( { l4,0,0 }) * math::rotate(q[5], { 1,0,0 });

		scene.setModels(f);
	}

	inline glm::vec3 getTranslationFromMouse(float xDiff, float yDiff) const
	{
		
		return 0.25f / camera.getZoomScale().y * invRotateZUp * glm::vec4((xDiff * camera.getRight() - yDiff * camera.getUp()), 0.0f);
	}

	void updateFromCursor()
	{
		if (!followCursor)
			return;

		simulation.setQ(solveInverseKinematics(simulation.getData().q1, cursor->getModel(), l1, l3, l4));
		dataChanged = true;
	}
};