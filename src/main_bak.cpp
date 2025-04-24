#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

// Forward declarations
void ShowBinaryParserApp();

int main()
{
    // Setup GLFW
    if (!glfwInit())
        return -1;

    // Setup OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Binary Parser Tool", NULL, NULL);
    if (!window)
        return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ShowBinaryParserApp();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// UI Code
void ShowBinaryParserApp()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("MainDockspace", nullptr, window_flags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
    if (!ImGui::DockBuilderGetNode(dockspace_id))
    {
        ImGui::DockBuilderRemoveNode(dockspace_id); // Clear previous
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        // Split into 2x2
        ImGuiID top, bottom, topLeft, topRight, bottomLeft, bottomRight;
        top = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.5f, nullptr, &bottom);
        topLeft = ImGui::DockBuilderSplitNode(top, ImGuiDir_Left, 0.5f, nullptr, &topRight);
        bottomLeft = ImGui::DockBuilderSplitNode(bottom, ImGuiDir_Left, 0.5f, nullptr, &bottomRight);

        ImGui::DockBuilderDockWindow("TopLeft", topLeft);
        ImGui::DockBuilderDockWindow("TopRight", topRight);
        ImGui::DockBuilderDockWindow("BottomLeft", bottomLeft);
        ImGui::DockBuilderDockWindow("BottomRight", bottomRight);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None);

    // ===== TOP BAR =====
    static int combo1 = 0;
    static int combo2 = 0;
    static char binPath[256] = "";

    ImGui::Begin("TopControls", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetNextItemWidth(150);
    ImGui::Combo("##Combo1", &combo1, "Option A\0Option B\0Option C\0");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    ImGui::Combo("##Combo2", &combo2, "Hex\0Dec\0Bin\0");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    ImGui::InputText("##BinPath", binPath, IM_ARRAYSIZE(binPath));
    ImGui::End();

    // ===== GRID PANELS =====
    ImGui::Begin("TopLeft");
    ImGui::Text("Top Left Panel");
    ImGui::End();

    ImGui::Begin("TopRight");
    ImGui::Text("Top Right Panel");
    ImGui::End();

    ImGui::Begin("BottomLeft");
    ImGui::Text("Bottom Left Panel");
    ImGui::End();

    ImGui::Begin("BottomRight");
    ImGui::Text("Bottom Right Panel");
    ImGui::End();

    ImGui::End(); // MainDockspace
}
