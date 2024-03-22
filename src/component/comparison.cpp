#include "comparison.hpp"

comparison_t::comparison_t(ImGuiKey key0, ImGuiKey key1, const size_t key):
    key0_(key0), key1_(key1), key_(key),
    title_(std::string(ImGui::GetKeyName(key0)) + " <-> " + ImGui::GetKeyName(key1)) {

}

bool comparison_t::process() {
    if (!active_)
        return false;

    {
        ImGui::Begin(title_.c_str(), &active_);
        ImGui::InputInt("Offset", &offset_);
        ImGui::SameLine(); HelpMarker(
                "You may set an offset to match relevant data points.\n"
                "Offset will be put on the right side key.\n"
                "e.g. Setting an offset of 1, will compare\n"
                "the n-th data on the left, and \n"
                "the (n+1)st of data on the right.\n"
                "The comparison range will be automatically clamped to\n"
                "valid range for both side of the comparison.");
        ImGui::End();
    }

    return true;
}
