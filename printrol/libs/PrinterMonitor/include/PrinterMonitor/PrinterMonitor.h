#pragma once

#include <string_view>
#include <vector>
#include <mutex>
#include <optional>

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


struct PrinterTemperature {
    float actual{ 0 }, set{ 0 };
    int power{ 0 };
};

class PrinterMonitor {
public:
    using temp_t = PrinterTemperature;
    using pos_t = std::vector<float>;
    using lck_t = std::unique_lock<std::mutex>;

    PrinterMonitor() {
        reset();
        parsers_.push_back(&PrinterMonitor::ok_parser);
        parsers_.push_back(&PrinterMonitor::parse_position);
        parsers_.push_back(&PrinterMonitor::parse_temperature);
        parsers_.push_back(&PrinterMonitor::parse_capability);
    }

    bool parse_line(std::string_view line);

    void reset();

    // position
    pos_t get_position() const {
        lck_t l(mtx_);
        return position_;
    }
    bool position_known() const {
        lck_t l(mtx_);
        return position_known_;
    }
    bool has_leveling() const {
        lck_t l(mtx_);
        return has_leveling_;
    }
    bool is_leveling_active() const {
        lck_t l(mtx_);
        return leveling_active_;
    }

    // temperature
    bool has_hotend(int index = 0) const {
        lck_t l(mtx_);
        return hotend_temps_.size() > (index + 1);
    }
    bool has_bed() const {
        lck_t l(mtx_);
        return bed_temp_.has_value();
    }
    bool has_chamber() const {
        lck_t l(mtx_);
        return chamber_temp_.has_value();
    }
    std::optional<temp_t> get_bed_temp() const {
        lck_t l(mtx_);
        return bed_temp_;
    }
    std::optional<temp_t> get_hotend_temp(int index = 0) const {
        lck_t l(mtx_);
        if (hotend_temps_.size() < index + 1) {
            return std::nullopt;
        }
        return hotend_temps_[index];
    }
    std::optional<temp_t> get_chamber_temp() const {
        lck_t l(mtx_);
        return chamber_temp_;
    }
    std::optional<temp_t> get_probe_temp() const {
        lck_t l(mtx_);
        return probe_temp_;
    }
    std::optional<temp_t> get_cooler_temp() const {
        lck_t l(mtx_);
        return cooler_temp_;
    }
    std::optional<temp_t> get_board_temp() const {
        lck_t l(mtx_);
        return board_temp_;
    }
    std::optional<temp_t> get_redundant_temp() const {
        lck_t l(mtx_);
        return redundant_temp_;
    }

    std::optional<PrinterCapabilities> get_capabilities() const {
        lck_t l(mtx_);
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
    mutable std::mutex mtx_;


    pos_t position_;
    bool position_known_;
    bool has_leveling_;
    bool leveling_active_;

    std::vector<temp_t> hotend_temps_;
    std::optional<temp_t> bed_temp_;
    std::optional<temp_t> chamber_temp_;
    std::optional<temp_t> probe_temp_;
    std::optional<temp_t> cooler_temp_;
    std::optional<temp_t> board_temp_;
    std::optional<temp_t> redundant_temp_;

    std::optional<PrinterCapabilities> capabilities_;
};
