#include "imcontext.hpp"
#include <limits>

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

ImPlotPoint imcontext::timeline_point_getter(int index, void* opaque) {
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


ImPlotPoint imcontext::distribution_point_getter(int index, void* opaque) {
    auto* session = static_cast<session_t*>(opaque);
    double x = (double)session->get_key();
    double y = session->get_durations()[index] * 1e-6;
    return { x, y };
}

bool imcontext::update() {
    ImGui::NewFrame();
    {
        ImGui::Begin("Framerate", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Render average %.3f ms/frame (%.1f fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::Button("Start a new session..."))
            show_pre_new_session = true;

        if (ImGui::Button("Start a new comparison..."))
            show_pre_new_comparison = true;

        ImGui::Checkbox("Show distribution", &show_distribution);
        ImGui::Checkbox("Show timeline", &show_timeline);
        if (show_timeline) {
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
        ImGui::Begin("Waiting for input...", &show_pre_new_session, ImGuiWindowFlags_AlwaysAutoResize);
        for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key = (ImGuiKey)(key + 1)) {
            if (key == ImGuiKey_MouseWheelX || key == ImGuiKey_MouseWheelY || key == ImGuiKey_MouseLeft) continue;
            if (funcs::IsLegacyNativeDupe(key)) continue;
            if (ImGui::IsKeyReleased(key)) {
                sessions.emplace(key, std::make_shared<session_t>(key, init_time));
                show_pre_new_session = false;
            }
        }
        ImGui::Text("Press a key to start a session for the key you pressed...");
        ImGui::Text("For mouse left, click the following button");
        if (ImGui::Button("New \'MouseLeft\' session...")) {
            sessions.emplace(ImGuiKey_MouseLeft, std::make_shared<session_t>(ImGuiKey_MouseLeft, init_time));
            show_pre_new_session = false;
        }
        ImGui::End();
    }

    if (show_pre_new_comparison) {
        //ImGui::ShowDemoWindow();

        ImGui::Begin("Choose two keys for comparison:", &show_pre_new_comparison, ImGuiWindowFlags_AlwaysAutoResize);
        
        if (ImGui::BeginListBox("##key0"))
        {
            for (auto& [key, session]: sessions) {
                const bool is_selected = (selected_key[0] == key);
                const bool is_disabled = (selected_key[1] == key);
                if (ImGui::Selectable(ImGui::GetKeyName(key), 
                    is_selected, is_disabled ? ImGuiSelectableFlags_Disabled : 0))
                    selected_key[0] = key;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }

        ImGui::SameLine();
        
        if (ImGui::BeginListBox("##key1"))
        {
            for (auto& [key, session]: sessions) {
                const bool is_selected = (selected_key[1] == key);
                const bool is_disabled = (selected_key[0] == key);
                if (ImGui::Selectable(ImGui::GetKeyName(key), 
                    is_selected, is_disabled ? ImGuiSelectableFlags_Disabled : 0))
                    selected_key[1] = key;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
        
        ImGui::BeginDisabled((selected_key[0] == ImGuiKey_None) || (selected_key[1] == ImGuiKey_None)); {
            if (ImGui::Button("Compare!")) {
                // Find if there's a hole
                auto it = std::find_if(comparisons.begin(), comparisons.end(), 
                    [](const comparison_t& c) { return !c.is_active(); });

                if (it != comparisons.end()) {
                    auto idx = std::distance(comparisons.begin(), it);
                    new (&comparisons[idx])
                        comparison_t(
                            selected_key[0], selected_key[1], idx,
                            sessions[selected_key[0]], sessions[selected_key[1]]);
                } else {
                    comparisons.emplace_back(
                        selected_key[0], selected_key[1], 
                        comparisons.size(),
                        sessions[selected_key[0]], sessions[selected_key[1]]);
                }
                show_pre_new_comparison = false;
            }
        }
        ImGui::EndDisabled();

        ImGui::End();
    } else {
        selected_key[0] = ImGuiKey_None;
        selected_key[1] = ImGuiKey_None;
    }

    for (auto it = sessions.begin(); it != sessions.end(); ) {
        if (!it->second->process()) {
            it = sessions.erase(it);
        } else {
            ++it;
        }
    }

    for (size_t i = 0; i < comparisons.size(); ++i) {
        comparisons[i].process();
    }

    if (show_timeline)
    {
        ImGui::Begin("Timeline", &show_timeline);

        ImVec2 v_min = ImGui::GetWindowContentRegionMin();
        ImVec2 v_max = ImGui::GetWindowContentRegionMax();

        v_min.x += ImGui::GetWindowPos().x;
        v_min.y += ImGui::GetWindowPos().y;
        v_max.x += ImGui::GetWindowPos().x;
        v_max.y += ImGui::GetWindowPos().y;

        if (ImPlot::BeginPlot("Timeline", ImVec2(v_max.x - v_min.x, v_max.y - v_min.y))) {
            if (first_update_) {
                ImPlot::SetupAxisLimits(ImAxis_Y1, -1.0, ImGuiKey_COUNT + 1.0);
            }

            ImPlot::SetupAxis(ImAxis_X1, "Time (ms)", (mode_ == display_mode_t::AutoFit) ? ImPlotAxisFlags_AutoFit : 0);
            ImPlot::SetupAxis(ImAxis_Y1, "Keycode * isReleased");
            if (mode_ == display_mode_t::TimeWindow)
            {
                auto now = (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - init_time);
                auto begin_time = now - time_window_size * 1e6;
                ImPlot::SetupAxisLimits(ImAxis_X1, begin_time * 1e-6, now * 1e-6, ImPlotCond_Always);
            }

            for (auto& [key, session]: sessions) {
                // The last index refers to the current('real-time') state
                ImPlot::PlotLineG(ImGui::GetKeyName(key),
                                  timeline_point_getter,
                                  session.get(),
                                  session->get_data_point_count() * 2 +
                                  ((real_time_update && !session->is_paused()) ? 1 : 0));
            }
            if (ImPlot::IsPlotHovered()) {
                ImPlotPoint point = ImPlot::GetPlotMousePos();
                // ImGui::Text("(%.2lf, %.2lf)", point.x, point.y);

                const ImPlotRect rect = ImPlot::GetPlotLimits(ImAxis_X1, ImAxis_Y1);
                double xmin = std::numeric_limits<double>::min();
                double xmax = std::numeric_limits<double>::max();
                //ImGui::BeginTooltip();
                for (auto& [key, session]: sessions) {
                    const auto next_idx = session->find_by_x(
                        (int64_t)point.x * 1e6, display_type_t::Released);
                    const auto last_idx = next_idx - 2;
                    
                    if (next_idx >= 0) {
                        const auto dp1 = session->get_data_point(next_idx);
                        const auto ts1 = dp1.timestamp;
                        xmax = std::min(xmax, ts1 * 1e-6);
                    }

                    if (last_idx >= 0) {
                        const auto dp0 = session->get_data_point(last_idx);
                        const auto ts0 = dp0.timestamp;
                        xmin = std::max(xmin, ts0 * 1e-6);
                    }

                    /*ImGui::Text("last %lld, next %lld, min %.2lf, max %.2lf", 
                                last_idx, next_idx, xmin, xmax);*/
                }
                //ImGui::EndTooltip();

                // Skip if on left or right edges
                // or even not on mouse pos
                if (xmin >= rect.X.Min && xmax <= rect.X.Max) {
                    double x_data[8] = {
                        xmin, xmin, xmin, xmin,
                        xmax, xmax, xmax, xmax
                    };
                    double y_data[8] = {
                        point.y, rect.Y.Max, rect.Y.Min, point.y,
                        point.y, rect.Y.Max, rect.Y.Min, point.y
                    };

                    ImPlot::PlotLine("##Hover", x_data, y_data, 8);

                    ImGui::BeginTooltip();
                    {
                        ImGui::Text("dT = %.2lfms", xmax - xmin);
                    }
                    ImGui::EndTooltip();
                }
            }
            ImPlot::EndPlot();
        }
        ImGui::End();
    }

    if (show_distribution)
    {
        ImGui::Begin("Distribution", &show_distribution);

        ImVec2 v_min = ImGui::GetWindowContentRegionMin();
        ImVec2 v_max = ImGui::GetWindowContentRegionMax();

        v_min.x += ImGui::GetWindowPos().x;
        v_min.y += ImGui::GetWindowPos().y;
        v_max.x += ImGui::GetWindowPos().x;
        v_max.y += ImGui::GetWindowPos().y;

        if (ImPlot::BeginPlot("Distribution", ImVec2(v_max.x - v_min.x, v_max.y - v_min.y))) {
            if (first_update_) {
                first_update_ = false;
                ImPlot::SetupAxisLimits(ImAxis_X1, -1.0, ImGuiKey_COUNT + 1.0);
            }
            ImPlot::SetupAxis(ImAxis_X1, "Keycode");
            ImPlot::SetupAxis(ImAxis_Y1, "Duration", ImPlotAxisFlags_AutoFit);
            for (auto& [key, session]: sessions) {
                ImPlot::PlotScatterG(ImGui::GetKeyName(session->get_key()), 
                    distribution_point_getter, session.get(), session->get_durations().size());
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

