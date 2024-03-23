#ifndef COMPONENT_COMPARISON_HPP
#define COMPONENT_COMPARISON_HPP

#include "imgui.h"
#include "implot.h"
#include <vector>
#include <chrono>
#include <string>
#include <unordered_map>
#include <memory>

#include "src/component/session.hpp"

class comparison_t {
public:
    comparison_t(
        const ImGuiKey key0, const ImGuiKey key1,
        const size_t key,
        std::shared_ptr<session_t> session0,
        std::shared_ptr<session_t> session1);
    bool process();
    const size_t get_key() { return key_; }
    bool is_active() const { return active_; }
private:
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

    bool active_ = true;
    const size_t key_ = 0;
    const ImGuiKey key0_ = ImGuiKey_None;
    const ImGuiKey key1_ = ImGuiKey_None;
    const std::string title_;
    std::shared_ptr<session_t> session0_;
    std::shared_ptr<session_t> session1_;
    int offset_ = 0;
    
    display_type_t display_type_[2] = {
        display_type_t::PressedAndReleased, 
        display_type_t::PressedAndReleased
    };
    const std::unordered_map<display_type_t, std::string> display_type_name = {
            { display_type_t::Pressed, "Press only" },
            { display_type_t::Released, "Release only" },
            { display_type_t::PressedAndReleased, "Press+Release" }
    };
};

#endif // COMPONENT_COMPARISON_HPP