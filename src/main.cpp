#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../include/imgui_memory_editor.h"

#include "../include/json.hpp"
using ordered_json = nlohmann::ordered_json;

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

        m_pt_tabBarflags        = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_DrawSelectedOverline | ImGuiWindowFlags_UnsavedDocument;
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
        if (ImGui::BeginTabBar(m_pt_strTabBaseId.c_str(), m_pt_tabBarflags))
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

class WidgetOutputTabManager
{
public:
    WidgetOutputTabManager()
    {
        m_pt_strJsonInputText = "{}";
        m_pt_ojsonObjJsonText = ordered_json::parse(m_pt_strJsonInputText);
    }

    void DrawJsonObject(const ordered_json &ojsonObjInput)
    {
        for (auto it = ojsonObjInput.begin(); it != ojsonObjInput.end(); ++it)
        {
            if (it->is_object())
            {
                if (ImGui::TreeNode(it.key().c_str()))
                {
                    DrawJsonObject(*it);
                    ImGui::TreePop();
                }
            }
            else if (it->is_array())
            {
                if (ImGui::TreeNode(it.key().c_str()))
                {
                    for (size_t i = 0; i < it->size(); ++i)
                    {
                        if ((*it)[i].is_object())
                        {
                            if (ImGui::TreeNode(std::to_string(i).c_str()))
                            {
                                DrawJsonObject((*it)[i]);
                                ImGui::TreePop();
                            }
                        }
                        else
                        {
                            ImGui::Text("[%zu]: %s", i, (*it)[i].dump().c_str());
                        }
                    }
                    ImGui::TreePop();
                }
            }
            else
            {
                ImGui::Text("%s: %s", it.key().c_str(), it->dump().c_str());
            }
        }
    }

    void Draw()
    {
        DrawJsonObject(m_pt_ojsonObjJsonText);
    }

    void SetJsonText(const std::string &strJsonText)
    {
        m_pt_strJsonInputText = strJsonText;
        m_pt_ojsonObjJsonText = ordered_json::parse(m_pt_strJsonInputText);
    }

protected:
    std::string m_pt_strJsonInputText;
    ordered_json m_pt_ojsonObjJsonText;
};

class WidgetJsonTable
{
public:
    WidgetJsonTable()
    {
        m_pt_strJsonInputText = "{}";
        m_pt_ojsonObjJsonText = ordered_json::parse(m_pt_strJsonInputText);
    }

    void DrawJsonObject(const ordered_json &ojsonObjInput)
    {
        for (auto it = ojsonObjInput.begin(); it != ojsonObjInput.end(); ++it)
        {
            if (it->is_object())
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::TreeNode(it.key().c_str()))
                {
                    DrawJsonObject(*it);
                    ImGui::TreePop();
                }
            }
            else if (it->is_array())
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::TreeNode(it.key().c_str()))
                {
                    for (size_t i = 0; i < it->size(); ++i)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        if ((*it)[i].is_object())
                        {
                            if (ImGui::TreeNode(std::to_string(i).c_str()))
                            {
                                DrawJsonObject((*it)[i]);
                                ImGui::TreePop();
                            }
                        }
                        else
                        {
                            ImGui::Text("[%zu]", i);
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", (*it)[i].dump().c_str());
                        }
                    }
                    ImGui::TreePop();
                }
            }
            else
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", it.key().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", it->dump().c_str());
            }
        }
    }

    void Draw()
    {
        if (ImGui::BeginTable("JSON Table", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("Key");
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();
            DrawJsonObject(m_pt_ojsonObjJsonText);
            ImGui::EndTable();
        }
    }

    void SetJsonText(const std::string &strJsonText)
    {
        m_pt_strJsonInputText = strJsonText;
        m_pt_ojsonObjJsonText = ordered_json::parse(m_pt_strJsonInputText);
    }

protected:
    std::string m_pt_strJsonInputText;
    ordered_json m_pt_ojsonObjJsonText;
};


class WidgetHexEditor
{
public:
    WidgetHexEditor()
    {
        m_pt_vtucBinaryData.push_back(0);
        // m_pt_sMemeoryEditor.DrawWindow("Hex Editor", m_pt_vtucBinaryData.data(), m_pt_vtucBinaryData.size());
    }

    void Draw(void)
    {
        m_pt_sMemeoryEditor.DrawContents(m_pt_vtucBinaryData.data(), m_pt_vtucBinaryData.size());
    }

    void SetBinaryData(const std::vector<unsigned char> &vtucBinaryData)
    {
        m_pt_vtucBinaryData.insert(m_pt_vtucBinaryData.begin(), vtucBinaryData.begin(), vtucBinaryData.end());
    }

protected:
    struct MemoryEditor m_pt_sMemeoryEditor;
    std::vector<unsigned char> m_pt_vtucBinaryData;
};

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
    
    WidgetTabInputManager objProgramTabWidget;

    WidgetTabInputManager objVariableTabWidget;
    objVariableTabWidget.SetTabBaseId("Variables");

    WidgetHexEditor objHexEditorWidget;

    std::string strJson = R"({
        "sSMARTInfo": {
            "ucCriticalWarning": {
                "ucSpareSpaceTrip": 0,
                "ucTemperatureTrip": 0,
                "ucReliabilityTrip": 0,
                "ucReadOnly": 0,
                "ucBackupCapFailed": 0,
                "ucRsvd": 0
            },
            "ucCriticalWarningRaw": 0
        }
    })";

    WidgetJsonTable objOutputJsonTableWindget;
    objOutputJsonTableWindget.SetJsonText(strJson);

    // WidgetOutputTabManager objOutputTabManager;
    // objOutputTabManager.SetJsonText(strJson);

    MemoryEditor sMemEditor;
    char display_data[10] = {0};
    int size = 0;


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

        ImGui::Begin("Hex Editor");
        objHexEditorWidget.Draw();
        ImGui::End();

        ImGui::Begin("Struct/Union Input");
        objProgramTabWidget.Draw();
        ImGui::End();

        ImGui::Begin("Variable Input");
        objVariableTabWidget.Draw();
        ImGui::End();

        // ImGui::Begin("Parsed Output");
        // objOutputTabManager.Draw();
        // ImGui::End();

        ImGui::Begin("Parsed Output");
        objOutputJsonTableWindget.Draw();
        ImGui::End();

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
