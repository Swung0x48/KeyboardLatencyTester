#ifndef KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
#define KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
#include "imgui.h"
#include "implot.h"
#include <unordered_map>
#include <src/component/session.hpp>
#include <windows.h>

namespace ImGui { extern ImGuiKeyData* GetKeyData(ImGuiKey key); }

class imcontext {
public:
    imcontext();
    bool update();
    ~imcontext();
    const ImVec4 background_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
private:
    static ImPlotPoint imcontext::data_point_getter(int index, void* opaque);
    std::unordered_map<ImGuiKey, session_t> sessions;

    bool show_demo_window = true;
    bool show_pre_new_session = false;
    bool show_keys = true;
    bool show_dashboard = true;
    static inline int64_t init_time;
    static inline int64_t last_poll_time;
    static inline int64_t delta_poll_time_;
};


#endif //KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
