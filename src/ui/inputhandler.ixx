module;
#include <GLFW/glfw3.h>
#include <imgui/imgui/imgui.h>

export module inputhandler;

import std;
import glm;

import camera;
import renderer;
import simulation;
import math;

export class InputHandler
{
public:
	InputHandler(GLFWwindow* window, Camera& camera, Renderer& renderer, SimulationEngine& simulation) :
		camera(camera), renderer(renderer), simulation(simulation)
	{
		glfwSetWindowUserPointer(window, this);

		glfwSetFramebufferSizeCallback(window, viewportCallback);
		glfwSetMouseButtonCallback(window, mouseCallback);
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetKeyCallback(window, keyboardCallback);
	}

	static void viewportCallback(GLFWwindow* window, int width, int height)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->renderer.setWindowSize(width, height);
		if (handler->renderer.splitScreen)
			width /= 2;
		handler->camera.setAspect(static_cast<float>(width) / static_cast<float>(height));
	}

	static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (ImGui::GetIO().WantCaptureMouse)
			return;

		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->handleMouseInput(window, button, action);
	}

	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->camera.zoom(static_cast<float>(yoffset));
	}
	static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));

		auto setKey = [&](bool value)
		{
			switch (key)
			{
			case GLFW_KEY_LEFT_SHIFT:
				handler->pressedKeys.SHIFT = value;
				break;
			case GLFW_KEY_LEFT_CONTROL:
				handler->pressedKeys.CTRL = value;
				break;
			case GLFW_KEY_A:
				handler->pressedKeys.A = value;
				break;
			case GLFW_KEY_Z:
				handler->pressedKeys.Z = value;
				break;
			case GLFW_KEY_S:
				handler->pressedKeys.S = value;
				break;
			case GLFW_KEY_X:
				handler->pressedKeys.X = value;
				break;
			case GLFW_KEY_D:
				handler->pressedKeys.D = value;
				break;
			case GLFW_KEY_C:
				handler->pressedKeys.C = value;
				break;
			case GLFW_KEY_F:
				handler->pressedKeys.F = value;
				break;
			case GLFW_KEY_V:
				handler->pressedKeys.V = value;
				break;
			case GLFW_KEY_G:
				handler->pressedKeys.G = value;
				break;
			case GLFW_KEY_B:
				handler->pressedKeys.B = value;
				break;
			case GLFW_KEY_H:
				handler->pressedKeys.H = value;
				break;
			case GLFW_KEY_N:
				handler->pressedKeys.N = value;
				break;
			}
		};

		if (action == GLFW_PRESS)
		{
			setKey(true);
		}
		else if (action == GLFW_RELEASE)
		{
			setKey(false);
		}
	}

	void handleMouseInput(GLFWwindow* window, int button, int action)
	{
		if (action == GLFW_PRESS)
		{
			switch (button)
			{
			case GLFW_MOUSE_BUTTON_RIGHT:
				mouseMovingMode = true;
				glfwGetCursorPos(window, &lastX, &lastY);
				initialX = lastX;
				initialY = lastY;
				break;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (button)
			{
			case GLFW_MOUSE_BUTTON_RIGHT:
				mouseMovingMode = false;
				break;
			}
		}
	}

	void handleMouseMovement(GLFWwindow* window)
	{
		if (!mouseMovingMode)
			return;

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		double dx = x - lastX;
		double dy = y - lastY;

		if ((dx < math::eps && dx > -math::eps) && (dy < math::eps && dy > -math::eps))
			return;

		int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

		float xDiff = dx / windowWidth * math::pi;
		float yDiff = dy / windowHeight * math::pi;

		// Camera movement and rotatione
		if (pressedKeys.CTRL)
		{
			if (pressedKeys.SHIFT)
			{
				renderer.moveCursor(xDiff, yDiff);
			}
			else
			{
				renderer.rotateCursor(xDiff, yDiff);
			}
		}
		else if (pressedKeys.SHIFT)
		{
			camera.move(xDiff, yDiff);
		}
		else
		{
			camera.rotate(xDiff, yDiff);
		}

		lastX = x;
		lastY = y;
	}

	void handleKeyboard(GLFWwindow* window)
	{
		if (simulation.isRunning())
			return;

		auto adjustQ = [&](int index, bool forward)
		{
			simulation.adjustQ(index, forward);
			renderer.signalDataChange();
		};

		if (pressedKeys.A)
			adjustQ(0, true);
		if (pressedKeys.Z)
			adjustQ(0, false);
		if (pressedKeys.S)
			adjustQ(1, true);
		if (pressedKeys.X)
			adjustQ(1, false);
		if (pressedKeys.D)
			adjustQ(2, true);
		if (pressedKeys.C)
			adjustQ(2, false);
		if (pressedKeys.F)
			adjustQ(3, true);
		if (pressedKeys.V)
			adjustQ(3, false);
		if (pressedKeys.G)
			adjustQ(4, true);
		if (pressedKeys.B)
			adjustQ(4, false);
		if (pressedKeys.H)
			adjustQ(5, true);
		if (pressedKeys.N)
			adjustQ(5, false);
	}

private:
	Camera& camera;
	Renderer& renderer;
	SimulationEngine& simulation;

	double lastX = 0;
	double lastY = 0;
	double initialX = 0;
	double initialY = 0;

	bool mouseMovingMode = false;

	struct PressedKeys
	{
		bool SHIFT = false;
		bool CTRL = false;
		bool A = false;
		bool Z = false;
		bool S = false;
		bool X = false;
		bool D = false;
		bool C = false;
		bool F = false;
		bool V = false;
		bool G = false;
		bool B = false;
		bool H = false;
		bool N = false;
	} pressedKeys;
};

