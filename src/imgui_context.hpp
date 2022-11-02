#ifndef KEYBOARDLATENCYTESTER_IMGUI_CONTEXT_HPP
#define KEYBOARDLATENCYTESTER_IMGUI_CONTEXT_HPP
#include "imgui.h"
#include <unordered_map>
#include <src/component/session.hpp>

namespace ImGui { extern ImGuiKeyData* GetKeyData(ImGuiKey key); }

class imgui_context {
public:
    bool init();
    bool update();
    const ImVec4 background_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
private:
    std::unordered_map<ImGuiKey, session_t> sessions;

    bool show_demo_window = true;
    bool show_pre_new_session = false;
    bool show_keys = true;
};


#endif //KEYBOARDLATENCYTESTER_IMGUI_CONTEXT_HPP
