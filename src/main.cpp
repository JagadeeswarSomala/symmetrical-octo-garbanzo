#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_memory_editor.h"

#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#ifdef __EMSCRIPTEN__
#include "imgui/examples/libs/emscripten/emscripten_mainloop_stub.h"
#endif

# include "widget_cmobo_box.hpp"

widget_combo_box *GetDriveFamilyComboBoxWidget(void)
{
    std::string drive_family_combo_name = "Drive Family";
    std::vector<std::string> drive_family_items = {"none", "Peregrine", "Phoenix-Ion", "Condor", "Raptor", "Raven", "Carbon", "Hydro"};

    widget_combo_box *pobj_drive_family_combo_box  = new widget_combo_box(drive_family_combo_name, drive_family_items);
    return pobj_drive_family_combo_box;
}

widget_combo_box *GetLogTypeComboBoxWidget(void)
{
    std::string log_type_combo_name = "Log Type";
    std::vector<std::string> log_type_items = {"none", "Supported Logs", "SMART", "FW Slot Info", "Host Telemetry", "Controller Telemetry", "Persistent", "Error Info"};

    widget_combo_box *pobj_log_type_combo_box  = new widget_combo_box(log_type_combo_name, log_type_items);
    return pobj_log_type_combo_box;
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    widget_combo_box *pobj_log_type_combo_box     = nullptr;
    widget_combo_box *pobj_drive_family_combo_box = nullptr;

    char binary_file_path[256] =  {'\0'};

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = nullptr;

#ifdef __EMSCRIPTEN__
    glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#else
    glsl_version = "#version 130"; // For desktop OpenGL 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    // Get the primary monitor's resolution
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "WBParser", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Drive Family Combo Box
    pobj_drive_family_combo_box = GetDriveFamilyComboBoxWidget();
    if (pobj_drive_family_combo_box == nullptr)
    {
        fprintf(stderr, "Failed to create Drive Family Combo Box");
        goto exit_status;
    }

    // Log Type Combo Box
    pobj_log_type_combo_box = GetLogTypeComboBoxWidget();
    if (pobj_log_type_combo_box == nullptr)
    {
        fprintf(stderr, "Failed to create Log Type Combo Box");
        goto exit_status;
    }

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Use full screen window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("MainWindow", nullptr,
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
        
        bool trigger_load = false;

        if (ImGui::BeginTable("ToolbarTable", 4, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoBordersInBody))
        {
            ImGui::TableSetupColumn("BinPath", ImGuiTableColumnFlags_WidthStretch, 700.0f); // 50%
            ImGui::TableSetupColumn("DriveFamily", ImGuiTableColumnFlags_WidthStretch, 250.0f); // 20%
            ImGui::TableSetupColumn("LogType", ImGuiTableColumnFlags_WidthStretch, 250.0f); // 20%

            ImGui::TableNextRow();

            // === Column 0: Binary File Path Input ===
            ImGui::TableSetColumnIndex(0);
            bool enter_pressed = ImGui::InputTextWithHint("##BinPath", "Enter binary file path", binary_file_path, sizeof(binary_file_path),
                                                        ImGuiInputTextFlags_EnterReturnsTrue);

            // Check if Enter was pressed
            if (enter_pressed)
                trigger_load = true;

            // === Load Button (Inline) ===
            ImGui::SameLine();
            if (ImGui::Button("Load"))
                trigger_load = true;

            // === Column 1: Drive Family Combo ===
            ImGui::TableSetColumnIndex(1);
            pobj_drive_family_combo_box->DrawComboBox();

            // === Column 2: Log Type Combo ===
            ImGui::TableSetColumnIndex(2);
            pobj_log_type_combo_box->DrawComboBox();

            ImGui::EndTable();
        }

        // === Shared Load Action (Once per frame) ===
        if (trigger_load)
        {
            // Replace this with actual loading logic
            printf("Loading file: %s\n", binary_file_path);
        }



        ImGui::End();

        /********** Rendering **********/
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

exit_status:
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    delete pobj_log_type_combo_box;
    delete pobj_drive_family_combo_box;

    return 0;
}
