#pragma once

#include <string_view>
#include <vector>

struct PrinterCapabilities {
    std::string FIRMWARE_NAME;
    std::string SOURCE_CODE_URL;
    std::string PROTOCOL_VERSION;
    std::string MACHINE_TYPE;
    int AXIS_COUNT{ 3 };
    std::string UUID;
    int EXTRUDER_COUNT{ 0 };
    bool PAREN_COMMENTS{ false };
    bool GCODE_QUOTED_STRINGS{ false };
    bool SERIAL_XON_XOFF{ false };
    bool BINARY_FILE_TRANSFER{ false };
    bool EEPROM{ false };
    bool VOLUMETRIC{ false };
    bool AUTOREPORT_POS{ false };
    bool AUTOREPORT_TEMP{ false };
    bool PROGRESS{ false };
    bool PRINT_JOB{ false };
    bool AUTOLEVEL{ false };
    bool RUNOUT{ false };
    bool Z_PROBE{ false };
    bool LEVELING_DATA{ false };
    bool BUILD_PERCENT{ false };
    bool SOFTWARE_POWER{ false };
    bool TOGGLE_LIGHTS{ false };
    bool CASE_LIGHT_BRIGHTNESS{ false };
    bool SPINDLE{ false };
    bool LASER{ false };
    bool EMERGENCY_PARSER{ false };
    bool HOST_ACTION_COMMANDS{ false };
    bool PROMPT_SUPPORT{ false };
    bool SDCARD{ false };
    bool MULTI_VOLUME{ false };
    bool REPEAT{ false };
    bool SD_WRITE{ false };
    bool AUTOREPORT_SD_STATUS{ false };
    bool LONG_FILENAME{ false };
    bool LFN_WRITE{ false };
    bool CUSTOM_FIRMWARE_UPLOAD{ false };
    bool EXTENDED_M20{ false };
    bool THERMAL_PROTECTION{ false };
    bool MOTION_MODES{ false };
    bool ARCS{ false };
    bool BABYSTEPPING{ false };
    bool CHAMBER_TEMPERATURE{ false };
    bool COOLER_TEMPERATURE{ false };
    bool MEATPACK{ false };
    bool CONFIG_EXPORT{ false };
};

class PrinterMonitor {
public:
    using temp_t = std::vector<float>;
    using pos_t = std::vector<float>;

    PrinterMonitor() {
        reset();
        parsers_.push_back(&PrinterMonitor::ok_parser);
        parsers_.push_back(&PrinterMonitor::parse_position);
        parsers_.push_back(&PrinterMonitor::parse_temperature);
        parsers_.push_back(&PrinterMonitor::parse_capability);
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

    PrinterCapabilities get_capabilities() const {
        return capabilities_;
    }

private:
    bool ok_parser();
    bool parse_position();
    bool parse_temperature();
    bool parse_capability();

    std::string current_line_;

    using parser_t = bool (PrinterMonitor::*)();
    std::vector<parser_t> parsers_;


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

    PrinterCapabilities capabilities_;
};
