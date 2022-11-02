#include "imgui_context.hpp"

bool imgui_context::init() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
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
    return true;
}

bool imgui_context::update() {
    ImGui::NewFrame();
    {
        ImGui::Begin("Framerate", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::Button("Start a new session..."))
            show_pre_new_session = true;
        ImGui::End();
    }

    if (show_pre_new_session) {
        ImGui::Begin("Waiting input...", &show_pre_new_session, ImGuiWindowFlags_AlwaysAutoResize);
        for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key = (ImGuiKey)(key + 1)) {
            if (key == ImGuiKey_MouseWheelX || key == ImGuiKey_MouseWheelY || key == ImGuiKey_MouseLeft) continue;
            if (funcs::IsLegacyNativeDupe(key)) continue;
            if (ImGui::IsKeyReleased(key)) {
                sessions.emplace(key, session_t{key});
                show_pre_new_session = false;
            }
        }
        ImGui::Text("Press a key to start a session for the key you pressed...");
        ImGui::Text("For mouse left, click the following button");
        if (ImGui::Button("New \'MouseLeft\' session...")) {
            sessions.emplace(ImGuiKey_MouseLeft, session_t{ImGuiKey_MouseLeft});
            show_pre_new_session = false;
        }
        ImGui::End();
    }

    for (auto it = sessions.begin(); it != sessions.end(); ) {
        if (!it->second.process()) {
            it = sessions.erase(it);
        } else {
            ++it;
        }
    }

    // Rendering
    ImGui::Render();
    return true;
}

