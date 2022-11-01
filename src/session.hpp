#ifndef SESSION_HPP
#define SESSION_HPP

#include "imgui.h"
#include <vector>
#include <chrono>
enum class keypress_type_t {
    Press,
    Release
};

struct record_t {
    keypress_type_t type;
    int64_t timestamp;
};

#ifdef IMGUI_DISABLE_OBSOLETE_KEYIO
    struct funcs { static bool IsLegacyNativeDupe(ImGuiKey) { return false; } };
    const ImGuiKey key_first = (ImGuiKey)ImGuiKey_NamedKey_BEGIN;
#else
    struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
    const ImGuiKey key_first = (ImGuiKey)0;
    //ImGui::Text("Legacy raw:");       for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key++) { if (io.KeysDown[key]) { ImGui::SameLine(); ImGui::Text("\"%s\" %d", ImGui::GetKeyName(key), key); } }
#endif

class session_t {
public:
    session_t(ImGuiKey key): 
        key(key) {}
    bool process();
    ImGuiKey get_key() { return key; }
private:
    bool active = true;
    bool paused = false;
    int64_t start_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    ImGuiKey key;
    std::vector<record_t> records;
};

#endif