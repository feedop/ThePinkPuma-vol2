#include <glad/glad.h>
#include <GLFW/glfw3.h>

import std;
import glm;

import application;
import camera;
import config;
import gui;
import simulation;
import renderer;
import inputhandler;
import math;
import scene;

#ifdef _WIN32
// Select an NVIDIA GPU if possible
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif

int main()
{
    Application application(cfg::initialWidth, cfg::initialHeight);
    GLFWwindow* window = application.getWindowPtr();
    // Create initial values for config and data
    SimulationConfig config
    {
        0.01f,
        5.0f,
        {0,0,0},
        {0,0,0},
        {1,0,0,0},
        {1,0,0,0},
    };

    // Create the simulation engine
    SimulationEngine simulationEngine;

    // Create a graphics controller
    Camera camera(10.0f);
    Scene scene1;
    Scene scene2;

    Renderer renderer(cfg::initialWidth, cfg::initialHeight, camera, simulationEngine, scene1, scene2);

    // Setup input handling
    InputHandler inputHandler(window, camera, renderer, simulationEngine);

    // Create a GUI controller
    GuiController guiController(window, camera, simulationEngine, renderer, config);

    // MAIN LOOP HERE - dictated by glfw
    double lastTime = glfwGetTime();
    int frameCount = 0;

    std::streambuf* coutSbuf = std::cerr.rdbuf(); // save original sbuf
    std::ofstream fout("/dev/null");
    std::cerr.rdbuf(fout.rdbuf()); // redirect 'cout' to a 'fout'

    while (!glfwWindowShouldClose(window))
    {    
        // Receive a snapshot of the data from the simulation
        SimulationData data = simulationEngine.getData();
        // OpenGL render
        renderer.draw(data);

        // GUI rendering
        guiController.draw(data);

        glfwSwapBuffers(window);

        // Show FPS in the title bar
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        if (delta >= 1.0)
        {
            double fps = double(frameCount) / delta;
            std::stringstream ss;
            ss << "The Pink Puma vol.2" << " " << " [" << fps << " FPS]";

            glfwSetWindowTitle(window, ss.str().c_str());
            lastTime = currentTime;
            frameCount = 0;
        }
        else
        {
            frameCount++;
        }

        // Handle user input
        glfwPollEvents();
        inputHandler.handleMouseMovement(window);
        inputHandler.handleKeyboard(window);
    }

    if (simulationEngine.isRunning())
        simulationEngine.stop();

    std::cerr.rdbuf(coutSbuf); // restore the original stream buffer
    return 0;
}