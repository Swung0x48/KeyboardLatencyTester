#include "session.hpp"
bool session_t::process() {
    if (!active_)
        return false;

    {
        ImGui::Begin(ImGui::GetKeyName(key_), &active_);
        if (ImGui::Button("Clear")) {
            records_.clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Paused", &paused_);
        ImGui::SameLine();
        if (ImGui::BeginCombo("##DisplayType", display_type_name.at(display_type_).c_str())) {
            for (const auto& [type, name]: display_type_name) {
                bool is_selected = (type == display_type_);
                if (ImGui::Selectable(name.c_str(), is_selected))
                    display_type_ = type;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }


        if (!paused_ && !funcs::IsLegacyNativeDupe(key_)) {
            if (ImGui::IsKeyDown(key_) && state_ == keystate_t::Released) {
                records_.emplace_back(record_t{keypress_type_t::Press, std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_time_ });
                state_ = keystate_t::Pressed;
            }
            if (!ImGui::IsKeyDown(key_) && state_ == keystate_t::Pressed) {
                records_.emplace_back(record_t{keypress_type_t::Release, std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_time_ });
                state_ = keystate_t::Released;
            }
        }

        if (ImGui::BeginTable(ImGui::GetKeyName(key_), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            for (auto& record : records_) {
                if (record.type == keypress_type_t::Press && ((int)display_type_ & (int)display_type_t::Pressed)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Press");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%lf", record.timestamp * 1e-6);
                }
                if (record.type == keypress_type_t::Release && ((int)display_type_ & (int)display_type_t::Released)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Release");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%lf", record.timestamp * 1e-6);
                }
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }

    return true;
}

record_t session_t::get_data_point(int index) {
    return records_[index];
}
