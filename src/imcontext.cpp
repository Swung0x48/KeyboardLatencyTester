#include "imcontext.hpp"

imcontext::imcontext() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
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
    init_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

ImPlotPoint imcontext::data_point_getter(int index, void* opaque) {
    auto* session = static_cast<session_t*>(opaque);
    double y = (double)session->get_key();

    if (session->get_data_point_count() * 2 == index) { // refers to current
        return {
            1e-6 * (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - init_time),
            (session->get_state() == keystate_t::Released) ? y : 0.0
        };
    }

    int actual_index = index / 2;
    int is_new_state = index % 2;
    auto point = session->get_data_point(actual_index);

    if ((is_new_state && point.type == keypress_type_t::Press) || (!is_new_state) && point.type == keypress_type_t::Release) {
        y = 0.0;
    }
    return { point.timestamp * 1e-6, y };
}

#include <iostream>
bool imcontext::update() {
    ImGui::NewFrame();
    {
        ImGui::Begin("Framerate", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Render average %.3f ms/frame (%.1f fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::Button("Start a new session..."))
            show_pre_new_session = true;

        ImGui::Checkbox("Show dashboard", &show_dashboard);
        if (show_dashboard) {
            ImGui::SameLine();
            ImGui::Checkbox("Real-time update", &real_time_update);
            if (ImGui::BeginCombo("Mode", mode_name.at(mode_).c_str())) {
                for (const auto&[mode, name]: mode_name) {
                    bool is_selected = (mode == mode_);
                    if (ImGui::Selectable(name.c_str(), is_selected))
                        mode_ = mode;
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (mode_ == display_mode_t::TimeWindow) {
                ImGui::SameLine();
                ImGui::InputDouble("ms", &time_window_size);
            }
        }
        ImGui::End();
    }

    if (show_pre_new_session) {
        ImGui::Begin("Waiting input...", &show_pre_new_session, ImGuiWindowFlags_AlwaysAutoResize);
        for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key = (ImGuiKey)(key + 1)) {
            if (key == ImGuiKey_MouseWheelX || key == ImGuiKey_MouseWheelY || key == ImGuiKey_MouseLeft) continue;
            if (funcs::IsLegacyNativeDupe(key)) continue;
            if (ImGui::IsKeyReleased(key)) {
                sessions.emplace(key, session_t{key, init_time});
                show_pre_new_session = false;
            }
        }
        ImGui::Text("Press a key to start a session for the key you pressed...");
        ImGui::Text("For mouse left, click the following button");
        if (ImGui::Button("New \'MouseLeft\' session...")) {
            sessions.emplace(ImGuiKey_MouseLeft, session_t{ImGuiKey_MouseLeft, init_time});
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

    if (show_dashboard)
    {
        ImGui::Begin("Dashboard", &show_dashboard);

        ImVec2 v_min = ImGui::GetWindowContentRegionMin();
        ImVec2 v_max = ImGui::GetWindowContentRegionMax();

        v_min.x += ImGui::GetWindowPos().x;
        v_min.y += ImGui::GetWindowPos().y;
        v_max.x += ImGui::GetWindowPos().x;
        v_max.y += ImGui::GetWindowPos().y;

        if (ImPlot::BeginPlot("Timeline", ImVec2(v_max.x - v_min.x, v_max.y - v_min.y))) {
            if (first_update_) {
                first_update_ = false;
                ImPlot::SetupAxisLimits(ImAxis_Y1, -1.0, ImGuiKey_COUNT + 1.0);
            }

            ImPlot::SetupAxis(ImAxis_X1, "Time (ms)", (mode_ == display_mode_t::AutoFit) ? ImPlotAxisFlags_AutoFit : 0);
            ImPlot::SetupAxis(ImAxis_Y1, "Keycode * isPressed");
            if (mode_ == display_mode_t::TimeWindow)
            {
                auto now = (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - init_time);
                auto begin_time = now - time_window_size * 1e6;
                ImPlot::SetupAxisLimits(ImAxis_X1, begin_time * 1e-6, now * 1e-6, ImPlotCond_Always);
            }

            for (auto& [key, session]: sessions) {
                // The last index refers to the current('real-time') state
                ImPlot::PlotLineG(ImGui::GetKeyName(key),
                                  data_point_getter,
                                  &session,
                                  session.get_data_point_count() * 2 + ((real_time_update && !session.is_paused()) ? 1 : 0));
            }
            ImPlot::EndPlot();
        }
        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    return true;
}

imcontext::~imcontext() {
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

