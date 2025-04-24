#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#ifdef __EMSCRIPTEN__
#include "../imgui/examples/libs/emscripten/emscripten_mainloop_stub.h"
#endif

class WidgetInputTextMultiline
{
public:
    WidgetInputTextMultiline() : m_pt_flags(0) {}

    WidgetInputTextMultiline(const std::string &strLabel, ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput, ImVec2 size = ImVec2(-FLT_MIN, 200))
        : m_pt_strLabel(strLabel), m_pt_flags(flags), m_pt_size(size) {}

    void SetLabel(const std::string &strLabel)
    {
        m_pt_strLabel = strLabel;
    }

    void SetFlags(ImGuiInputTextFlags flags)
    {
        m_pt_flags = flags;
    }

    void SetSize(float width = -FLT_MIN, float height = 200)
    {
        m_pt_size.x = width;
        m_pt_size.y = height;
    }

    std::string & GetInputData()
    {
        return m_pt_strInputData;
    }

    const std::string & GetInputData() const
    {
        return m_pt_strInputData;
    }

    void Draw()
    {
        ImGui::InputTextMultiline(m_pt_strLabel.c_str(), &m_pt_strInputData, m_pt_size, m_pt_flags);
    }

protected:
    std::string m_pt_strLabel;
    ImVec2 m_pt_size;
    ImGuiInputTextFlags m_pt_flags = 0;

    std::string m_pt_strInputData;
};

class WidgetTabInputManager
{
    public:
    WidgetTabInputManager()
    {
        m_pt_strTabBaseId = "Program";
        m_pt_strTextInputLabel = "##struct";

        m_pt_tabBarflags        = ImGuiTabBarFlags_Reorderable;
        m_pt_textInputflags     = ImGuiInputTextFlags_AllowTabInput;
    }

    void SetTabBaseId(const std::string &strTabBaseId = "Program")
    {
        m_pt_strTabBaseId = strTabBaseId;
    }

    void SetTextInputLable(const std::string &strTextInputLabel = "##struct")
    {
        m_pt_strTextInputLabel = strTextInputLabel;
    }

    void SetFlags(ImGuiTabBarFlags tabBarflags, ImGuiInputTextFlags textInputflags)
    {
        m_pt_tabBarflags = tabBarflags;
        m_pt_textInputflags = textInputflags;
    }

    void Draw()
    {
        if (ImGui::BeginTabBar(m_pt_strTabBaseId.c_str(), ImGuiTabBarFlags_Reorderable))
        {
            // Draw each tab
            for (size_t i = 0; i < m_tabs.size(); ++i)
            {
                std::string tabLabel = m_pt_strTabBaseId.c_str() + std::string(" ") + std::to_string(i + 1);
                if (ImGui::BeginTabItem(tabLabel.c_str()))
                {
                    m_tabs[i].Draw();
                    ImGui::EndTabItem();
                }
            }

            // "+" Button to add new tab
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            {
                AddNewTab();
            }

            ImGui::EndTabBar();
        }
    }

    void AddNewTab()
    {
        WidgetInputTextMultiline newWidget(m_pt_strTextInputLabel, ImGuiInputTextFlags_AllowTabInput);
        m_tabs.push_back(newWidget);
    }

    protected:
    std::string m_pt_strTabBaseId;
    std::string m_pt_strTextInputLabel;

    ImGuiTabBarFlags m_pt_tabBarflags;
    ImGuiInputTextFlags m_pt_textInputflags;

    std::vector<WidgetInputTextMultiline> m_tabs;
};


void DrawTab()
{
    static ImVector<int> active_tabs;
    static int next_tab_id = 0;

    static bool show_help_button = true;
    static bool show_add_tab_button = true;

    bool opened = true;
    static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;

    if (ImGui::BeginTabBar("ProgramInput", tab_bar_flags))
    {
        if (show_help_button)
        {
            if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
            {
                ImGui::OpenPopup("ProgramInputHelp");

                if (ImGui::BeginPopup("ProgramInputHelp"))
                {
                    ImGui::Selectable("Program Input");
                    ImGui::EndPopup();
                }
            }

            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            {
                active_tabs.push_back(next_tab_id++); // Add new tab
            }

            for (int n = 0; n < active_tabs.Size; )
            {
                bool open = true;

                if (ImGui::BeginTabItem("Struct", &open, ImGuiTabItemFlags_None))
                {
                    static char text[1024 * 16] =
                        "/*\n"
                        " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
                        " the hexadecimal encoding of one offending instruction,\n"
                        " more formally, the invalid operand with locked CMPXCHG8B\n"
                        " instruction bug, is a design flaw in the majority of\n"
                        " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
                        " processors (all in the P5 microarchitecture).\n"
                        "*/\n\n"
                        "label:\n"
                        "\tlock cmpxchg8b eax\n";

                    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
                    ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

                    ImGui::EndTabItem();
                }
            }
        }

                



        ImGui::EndTabBar();
    }
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // WidgetInputTextMultiline objTextInput("##Struct_1", ImGuiInputTextFlags_AllowTabInput);
    
    WidgetTabInputManager objTabWidget;

    // Main loop
#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
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

        // objTextInput.Draw();
        // printf("%s\n", objTextInput.GetInputData().c_str());

        objTabWidget.Draw();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
