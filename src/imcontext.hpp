#ifndef KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
#define KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
#include "imgui.h"
#include "implot.h"
#include <unordered_map>
#include <src/component/session.hpp>

namespace ImGui { extern ImGuiKeyData* GetKeyData(ImGuiKey key); }

class imcontext {
public:
    imcontext();
    bool update();
    ~imcontext();
    const ImVec4 background_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
private:
    std::unordered_map<ImGuiKey, session_t> sessions;

    bool show_demo_window = true;
    bool show_pre_new_session = false;
    bool show_keys = true;
    bool show_dashboard = true;
    bool auto_scroll = true;
    int64_t init_time;
};


#endif //KEYBOARDLATENCYTESTER_IMCONTEXT_HPP
