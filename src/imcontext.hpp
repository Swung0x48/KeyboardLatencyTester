#ifndef KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
#define KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
#include "imgui.h"
#include "implot.h"
#include <unordered_map>
#include <src/component/session.hpp>

namespace ImGui { extern ImGuiKeyData* GetKeyData(ImGuiKey key); }

enum class display_mode_t {
    Manual,
    AutoFit,
    TimeWindow
};

class imcontext {
public:
    imcontext();
    bool update();
    ~imcontext();
    const ImVec4 background_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
private:
    static ImPlotPoint timeline_point_getter(int index, void* opaque);
    static ImPlotPoint distribution_point_getter(int index, void* opaque);
    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
    std::unordered_map<ImGuiKey, session_t> sessions;

    bool first_update_ = true;
    bool show_demo_window = true;
    bool show_pre_new_session = false;
    bool show_pre_new_comparison = false;
    ImGuiKey selected_key[2] = { ImGuiKey_None, ImGuiKey_None };
    bool show_keys = true;
    bool show_timeline = true;
    bool show_distribution = true;
    bool real_time_update = false;
    display_mode_t mode_ = display_mode_t::Manual;
    const std::unordered_map<display_mode_t, std::string> mode_name = {
            { display_mode_t::Manual, "Manual" },
            { display_mode_t::AutoFit, "Auto-fit" },
            { display_mode_t::TimeWindow, "Time-window" }
    };
//    bool realtime_mode = true;
//    bool timewindow_mode = false;
    double time_window_size = 10 * 1e3;
    static inline int64_t init_time;
};


#endif //KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
