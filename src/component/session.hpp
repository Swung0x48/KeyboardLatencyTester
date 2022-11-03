#ifndef SESSION_HPP
#define SESSION_HPP

#include "imgui.h"
#include "implot.h"
#include <vector>
#include <chrono>
enum class keypress_type_t {
    Press,
    Release
};

enum class keystate_t {
    Pressed,
    Released
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
    session_t(ImGuiKey key, int64_t init_time):
        key(key), start_time(init_time), state(ImGui::IsKeyDown(key) ? keystate_t::Pressed : keystate_t::Released) {}
    bool process();
    ImGuiKey get_key() { return key; }
    record_t get_data_point(int);
    size_t get_data_point_count() { return records.size(); }
    keystate_t get_state() { return state; }
    bool is_paused() { return paused; }
private:
    bool active = true;
    bool paused = false;
    int64_t start_time = 0;
    ImGuiKey key;
    keystate_t state;
    std::vector<record_t> records;
};

#endif