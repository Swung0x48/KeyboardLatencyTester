#ifndef KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
#define KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
#include "imgui.h"
#include "implot.h"
#include <unordered_map>
#include <src/component/session.hpp>
#include <windows.h>

namespace ImGui { extern ImGuiKeyData* GetKeyData(ImGuiKey key); }

enum class mode_t {
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
    static ImPlotPoint imcontext::data_point_getter(int index, void* opaque);
    std::unordered_map<ImGuiKey, session_t> sessions;

    bool first_update_ = true;
    bool show_demo_window = true;
    bool show_pre_new_session = false;
    bool show_keys = true;
    bool show_dashboard = true;
    bool real_time_update = false;
    mode_t mode_ = mode_t::Manual;
    const std::unordered_map<mode_t, std::string> mode_name = {
            { mode_t::Manual, "Manual" },
            { mode_t::AutoFit, "Auto-fit" },
            { mode_t::TimeWindow, "Time-window" }
    };
//    bool realtime_mode = true;
//    bool timewindow_mode = false;
    double time_window_size = 1e3;
    static inline int64_t init_time;
};


#endif //KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
