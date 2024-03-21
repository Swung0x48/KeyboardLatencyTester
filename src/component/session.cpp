#include "session.hpp"
#include <fstream>
#include <cmath>
#include "implot_internal.h"

bool session_t::process() {
    if (!active_)
        return false;

    {
        ImGui::Begin(ImGui::GetKeyName(key_), &active_);
        if (ImGui::Button("Clear")) {
            records_.clear();
            press_indices_.clear();
            release_indices_.clear();
            //press_count_ = 0;
            //release_count_ = 0;
            accumulated_pressed_time_ = 0;
            accumulated_pressed_time_squared = 0;
            durations_.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save as csv")) {
            auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::string filename = std::string(ImGui::GetKeyName(key_)) + "_" + std::to_string(now) + ".csv";
            std::ofstream stream(filename, std::ios::out | std::ios::trunc);
            to_csv(stream);
            stream.close();
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

        {
            ImGui::Text("Press: %d", press_indices_.size());
            ImGui::SameLine();
            ImGui::Text("Release: %d", release_indices_.size());
            ImGui::SameLine();
            ImGui::Text("Total: %llu", records_.size());
            if (release_indices_.empty())
                ImGui::Text("Duration stats not available.");
            else {
                size_t n = release_indices_.size();
                double avg = accumulated_pressed_time_ * 1.0 / n;
                double stddev = sqrt(accumulated_pressed_time_squared * 1.0 / n - avg * avg);
                ImGui::Text("Duration mean: %.2lfms | stddev: %.2lfms",
                            avg * 1e-6, stddev * 1e-6);
            }
        }


        if (!paused_ && !funcs::IsLegacyNativeDupe(key_)) {
            if (ImGui::IsKeyDown(key_) && state_ == keystate_t::Released) {
                records_.emplace_back(record_t{keypress_type_t::Press, std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_time_ });
                press_indices_.emplace_back(records_.size() - 1);
                
                state_ = keystate_t::Pressed;
                //++press_count_;
            }
            if (!ImGui::IsKeyDown(key_) && state_ == keystate_t::Pressed) {
                records_.emplace_back(record_t{keypress_type_t::Release, std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_time_ });
                release_indices_.emplace_back(records_.size() - 1);
                
                state_ = keystate_t::Released;
                //++release_count_;
                size_t n = records_.size();
                if (n >= 2) {
                    int64_t x = records_[n - 1].timestamp - records_[n - 2].timestamp;
                    accumulated_pressed_time_ += x;
                    accumulated_pressed_time_squared += (x * x);
                    durations_.emplace_back(x);
                }
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

record_t session_t::get_data_point(const size_t index) {
    return records_[index];
}

int64_t session_t::find_by_x(const int64_t x, const display_type_t type) {
    switch (type) {
        case display_type_t::PressedAndReleased: {
            auto it = std::lower_bound(records_.begin(), records_.end(), x,
            [](const record_t& record, const int64_t val) {
                return record.timestamp < val;
            });
        
            if (it == records_.end())
                return -1;
            
            return std::distance(records_.begin(), it);
        }
        case display_type_t::Pressed: {
            auto it = std::lower_bound(press_indices_.begin(), press_indices_.end(), x,
            [this](const size_t idx, const int64_t val) {
                const record_t& record = records_[idx];
                return record.timestamp < val;
            });

            if (it == press_indices_.end())
                return -1;
            
            return *it;
        }
        case display_type_t::Released: {
            auto it = std::lower_bound(release_indices_.begin(), release_indices_.end(), x,
            [this](const size_t idx, const int64_t val) {
                const record_t& record = records_[idx];
                return record.timestamp < val;
            });

            if (it == release_indices_.end())
                return -1;
            
            return *it;
        }
        default:
            return -1;
    }
}

void session_t::to_csv(std::ostream& stream) {
    stream << "Timestamp,isPressed?\n";
    for (auto& record: records_) {
        stream << record.timestamp + start_time_ << "," << (int)record.type << "\n";
    }
}