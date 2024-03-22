#include "comparison.hpp"

comparison_t::comparison_t(
    ImGuiKey key0, ImGuiKey key1, 
    const size_t key,
    std::shared_ptr<session_t> session0,
    std::shared_ptr<session_t> session1):
    key0_(key0), key1_(key1), key_(key),
    title_(std::string(ImGui::GetKeyName(key0)) + " <-> " + ImGui::GetKeyName(key1)),
    session0_(session0), session1_(session1) {

}

bool comparison_t::process() {
    if (!active_)
        return false;

    {
        ImGui::Begin(title_.c_str(), &active_);
        
        {
            ImGui::InputInt("Offset", &offset_);
            ImGui::SameLine(); HelpMarker(
                    "You may set an offset to match relevant data points.\n"
                    "Offset will be put on the right side key.\n"
                    "e.g. Setting an offset of 1, will compare\n"
                    "the n-th data on the left, and \n"
                    "the (n+1)st of data on the right.\n"
                    "The comparison range will be automatically clamped to\n"
                    "valid range for both side of the comparison.");
        }

        if (ImGui::BeginTable(title_.c_str(), 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            size_t len = std::min(session0_->get_data_point_count(), session1_->get_data_point_count());
            for (size_t i = 0; i < len; ++i) {
                ImGui::TableNextRow();
                const auto& record0 = session0_->get_data_point(i);
                const auto& record1 = session1_->get_data_point(i);

                std::string str0(ImGui::GetKeyName(key0_));
                std::string str1(ImGui::GetKeyName(key1_));

                str0 += std::to_string(i);
                str1 += std::to_string(i);

                str0 += (record0.type == keypress_type_t::Press) ? " \\" : " /";
                str1 += (record1.type == keypress_type_t::Press) ? " \\" : " /";
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", str0.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", str1.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%+lf", (record0.timestamp - record1.timestamp) * 1e-6);
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }

    return true;
}
