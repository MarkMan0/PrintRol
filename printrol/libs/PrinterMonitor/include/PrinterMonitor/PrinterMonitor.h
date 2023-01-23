#pragma once

#include <string_view>
#include <vector>

class PrinterMonitor {
public:
    using temp_t = std::vector<float>;
    using pos_t = std::vector<float>;

    PrinterMonitor() {
        reset();
    }

    void parse_line(std::string_view line);

    void reset();

    // position
    pos_t get_position() const {
        return position_;
    }
    bool position_known() const {
        return position_known_;
    }
    bool has_leveling() const {
        return has_leveling_;
    }
    bool is_leveling_active() const {
        return leveling_active_;
    }

    // temperature
    bool has_hotend(int index = 0) const {
        return hotend_temps_.size() > (index + 1);
    }
    bool has_bed() const {
        return bed_temp_.size() > 0;
    }
    bool has_chamber() const {
        return chamber_temp_.size() > 0;
    }
    temp_t get_bed_temp() const {
        return bed_temp_;
    }
    temp_t get_hotend_temp(int index = 0) const {
        if (hotend_temps_.size() < index + 1) {
            return {};
        }
        return hotend_temps_[index];
    }
    temp_t get_chamber_temp() const {
        return chamber_temp_;
    }
    temp_t get_probe_temp() const {
        return probe_temp_;
    }
    temp_t get_cooler_temp() const {
        return cooler_temp_;
    }
    temp_t get_board_temp() const {
        return board_temp_;
    }
    temp_t get_redundant_temp() const {
        return redundant_temp_;
    }

private:
    void parse_position();
    void parse_temperature();

    std::string current_line_;

    pos_t position_;
    bool position_known_;
    bool has_leveling_;
    bool leveling_active_;

    std::vector<temp_t> hotend_temps_;
    temp_t bed_temp_;
    temp_t chamber_temp_;
    temp_t probe_temp_;
    temp_t cooler_temp_;
    temp_t board_temp_;
    temp_t redundant_temp_;
};
