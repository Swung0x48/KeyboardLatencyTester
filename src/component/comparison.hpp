#ifndef COMPONENT_COMPARISON_HPP
#define COMPONENT_COMPARISON_HPP

#include "imgui.h"
#include "implot.h"
#include <vector>
#include <chrono>
#include <string>
#include <unordered_map>

#include "src/component/session.hpp"

class comparison_t {
public:
    comparison_t(
        const ImGuiKey key0, const ImGuiKey key1, const size_t key);
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
    int offset_ = 0;
};

#endif // COMPONENT_COMPARISON_HPP