module;
#include <GLFW/glfw3.h>
#include <imgui/imgui/imgui.h>
#include <imgui/backend/imgui_impl_glfw.h>
#include <imgui/backend/imgui_impl_opengl3.h>

export module gui:controller;

import std;
import glm;

import camera;
import renderer;
import simulation;
import theme;
import imguiext;
import marker;
import inputhandler;
import math;

ImGuiIO& createImguiContext()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    return ImGui::GetIO();
}

/// <summary>
/// Responsible for managing the GUI
/// </summary>
export class GuiController
{
public:
    GuiController(GLFWwindow* window, Camera& camera, SimulationEngine& simulation, Renderer& renderer, const SimulationConfig& config) :
        io(createImguiContext()), window(window), camera(camera), simulation(simulation), renderer(renderer), config(config)
    {
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowMinSize = ImVec2(500, 500);

        cinder::setTheme();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 420");
    }

    void draw(const SimulationData& data)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // imgui debug
        bool showDemo = false;
        if (showDemo)
        {
            ImGui::ShowDemoWindow(&showDemo);
            render();
            return;
        }

        // Render the config window
        const float PAD = 10.0f;
        const int location = 0;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowBgAlpha(0.3f);
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(350, 700));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

        ImGui::Begin("Configuration window", nullptr, window_flags);
        renderConfig(data);
        ImGui::End();

        render();
    }
private:
    ImGuiIO& io;
    GLFWwindow* window;
    Camera& camera;
    SimulationEngine& simulation;
    Renderer& renderer;

    SimulationConfig config;

    static constexpr int size = 144 * 20;

    void render()
    {
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    // Components
    void renderConfig(const SimulationData& data)
    {
        ImGui::Text("Display settings:");
        static bool fpsLimit = true;
        if (ImGui::Checkbox("Limit fps to screen frequency", &fpsLimit))
        {
            glfwSwapInterval(fpsLimit);
        }
        if (ImGui::Checkbox("Split screen", &renderer.splitScreen))
        {
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            InputHandler::viewportCallback(window, width, height);
        }
        ImGui::Checkbox("Show grid", &renderer.drawGrid);
        ImGui::Checkbox("Follow cursor", &renderer.followCursor);
        ImGui::Checkbox("Shaded puma", &renderer.shadedPuma);
        ImGui::Checkbox("Markers always on top", &renderer.markersOnTop);
        ImGui::Checkbox("Render extension marker", &renderer.renderExtensionMarker);
        ImGui::Checkbox("Render local coordinate base axes", &Marker::baseCursorAxes);
        ImGui::Checkbox("Unique colors for coordinate systems", &Marker::uniqueColors);
        if (Marker::uniqueColors)
        {
            ImGui::Checkbox("Extend marker axes", &Marker::extendAxes);
        }
        ImGui::NewLine();

        if (simulation.isRunning())
        {
            ImGui::Text(std::format("Time: {} out of {}", data.t, config.totalTime).c_str());

            if (ImGui::Button("Stop simulation"))
            {
                simulation.stop();
            }
            return;
        }

        ImGui::Text("General simulation settings:");
        ImGui::SliderFloat("dt", &config.dt, 0.001f, 1.0f);
        ImGui::SliderFloat("total time", &config.totalTime, 0.0001f, 60.0f);

        ImGui::NewLine();
        bool dataChanged = false;
        dataChanged |= ImGui::SliderFloat("l1", &renderer.l1, 0.2f, 5.0f);
        dataChanged |= ImGui::SliderFloat("l3", &renderer.l3, 0.2f, 5.0f);
        dataChanged |= ImGui::SliderFloat("l4", &renderer.l4, 0.2f, 5.0f);

        ImGui::NewLine();
        ImGui::Text("Position (x,y,z)");
        ImGui::InputFloat3("Start position", reinterpret_cast<float*>(&config.startPosition));
        ImGui::InputFloat3("End position", reinterpret_cast<float*>(&config.endPosition));

        static glm::vec3 startRotation{ 0,0,0 };
        static glm::vec3 endRotation{ 0,0,0 };
        ImGui::Text("Rotation - Euler angles (degrees)");
        ImGui::InputFloat3("Start rotation", reinterpret_cast<float*>(&startRotation));
        ImGui::InputFloat3("End rotation", reinterpret_cast<float*>(&endRotation));

        if (ImGui::Button("Set start from cursor"))
        {
            setConfigFromCursorModel(renderer.getCursorModel(), config.startPosition, config.startQuat);
            startRotation = math::degrees(glm::eulerAngles(config.startQuat));
        }
        if (ImGui::Button("Set end from cursor"))
        {
            setConfigFromCursorModel(renderer.getCursorModel(), config.endPosition, config.endQuat);
            endRotation = math::degrees(glm::eulerAngles(config.endQuat));
        }

        if (dataChanged)
        {
            renderer.signalDataChange();
        }

        if (ImGui::Button("Run animation"))
        {
            renderer.passLToSimulation();
            config.startQuat = glm::quat(math::radians(startRotation));
            config.endQuat = glm::quat(math::radians(endRotation));
            simulation.setConfig(config);           
            simulation.start();
        }
    }

    void setConfigFromCursorModel(const glm::mat4& model, glm::vec3& position, glm::quat& rotation)
    {
        position = model[3];
        glm::mat3 rotationMatrix = model;

        // Normalize the rotation matrix to remove scaling (if any)
        rotationMatrix[0] = glm::normalize(rotationMatrix[0]);
        rotationMatrix[1] = glm::normalize(rotationMatrix[1]);
        rotationMatrix[2] = glm::normalize(rotationMatrix[2]);

        // Convert the rotation matrix to a quaternion
        rotation = glm::quat_cast(rotationMatrix);
    }
};