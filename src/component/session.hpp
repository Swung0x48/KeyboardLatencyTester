#ifndef SESSION_HPP
#define SESSION_HPP

#include "imgui.h"
#include "implot.h"
#include <vector>
#include <chrono>
#include <unordered_map>
#include <sstream>
enum class keypress_type_t: int {
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

enum class display_type_t: uint32_t {
    Pressed = 1,
    Released = 1 << 1,
    PressedAndReleased = 1 | (1 << 1)
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
            key_(key), start_time_(init_time), state_(ImGui::IsKeyDown(key) ? keystate_t::Pressed : keystate_t::Released) {}
    bool process();
    void to_csv(std::ostream& stream);
    ImGuiKey get_key() { return key_; }
    record_t get_data_point(const size_t);
    int64_t find_by_x(const int64_t, const display_type_t type = display_type_t::PressedAndReleased);
    size_t get_data_point_count() { return records_.size(); }
    keystate_t get_state() { return state_; }
    bool is_paused() { return paused_; }
    const std::vector<int64_t>& get_durations() { return durations_; }
private:
    bool active_ = true;
    bool paused_ = false;
    int64_t start_time_ = 0;
    // int press_count_ = 0;
    // int release_count_ = 0;
    int64_t accumulated_pressed_time_ = 0;
    int64_t accumulated_pressed_time_squared = 0;
    ImGuiKey key_;
    keystate_t state_;
    display_type_t display_type_ = display_type_t::PressedAndReleased;
    const std::unordered_map<display_type_t, std::string> display_type_name = {
            { display_type_t::Pressed, "Press only" },
            { display_type_t::Released, "Release only" },
            { display_type_t::PressedAndReleased, "Press+Release" }
    };
    std::vector<record_t> records_;
    std::vector<size_t> press_indices_;
    std::vector<size_t> release_indices_;
    std::vector<int64_t> durations_;
};

#endif