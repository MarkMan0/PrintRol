#include "PrinterMonitor/PrinterMonitor.h"
#include <regex>
#include <array>
#include <tuple>
#include <sstream>

void PrinterMonitor::reset() {
    position_.clear();
    position_known_ = false;
    has_leveling_ = false;
    leveling_active_ = false;

    hotend_temps_.clear();
    bed_temp_.clear();
    chamber_temp_.clear();
    probe_temp_.clear();
    cooler_temp_.clear();
    board_temp_.clear();
    redundant_temp_.clear();
}

void PrinterMonitor::parse_line(std::string_view line) {
    current_line_ = line;

    for (const auto& parser : parsers_) {
        if ((this->*parser)()) {
            // break;
        }
    }
}


bool PrinterMonitor::parse_position() {
    const std::string regex_ending = "([+-]?\\d*.?\\d*).*$";
    std::array<std::string, 4> axis_names = { "X:", "Y:", "Z:", "E:" };
    pos_t pos2;
    pos2.reserve(4);
    for (const auto& name : axis_names) {
        const std::string rgx = name + regex_ending;
        std::smatch m;
        bool res = std::regex_search(current_line_, m, std::regex(rgx));
        if (not res || m.size() < 2) {
            break;
        }
        std::string match(m[1]);

        try {
            float pos = std::stof(match);
            pos2.push_back(pos);
        } catch (...) {
        }
    }

    if (pos2.size() == 4) {
        position_ = std::move(pos2);
    }

    return true;
}


bool PrinterMonitor::parse_temperature() {
    const std::string num_matcher = "(\\d*.?\\d*) \\/(\\d*.?\\d*)";

    const bool has_multi_hotend = current_line_.find("T0:") != std::string::npos;

    // matcher
    auto match = [&num_matcher, this](char prefix_1, char prefix_2, int hotend_num = -1) -> temp_t {
        std::smatch m;
        std::string actual, target, power;

        std::stringstream ss1;
        ss1 << prefix_1;
        if (hotend_num != -1) {
            ss1 << hotend_num;
        }
        ss1 << ":" << num_matcher;
        std::stringstream ss2;
        ss2 << prefix_2 << "@";
        if (hotend_num != -1) {
            ss2 << hotend_num;
        }
        ss2 << ":(\\d*)";
        if (std::regex_search(current_line_, m, std::regex(ss1.str())) && m.size() > 2) {
            actual = m[1];
            target = m[2];
            if (std::regex_search(current_line_, m, std::regex(ss2.str())) && m.size() > 1) {
                power = m[1];
            } else {
                power = "0";
            }
        }
        try {
            return { std::stof(actual), std::stof(target), std::stof(power) };
        } catch (...) {
            return {};
        }
    };

    hotend_temps_.clear();
    // check multiple hotends
    int i = 0;
    while (1) {
        // push back hotends while they exist
        auto ret = match('T', ' ', i++);
        if (ret.size()) {
            hotend_temps_.push_back(ret);
        } else {
            break;
        }
    }
    // will only match with single hotend systems
    hotend_temps_.push_back(match('T', ' '));
    bed_temp_ = match('B', 'B');
    chamber_temp_ = match('C', 'C');
    probe_temp_ = match('P', 'P');
    cooler_temp_ = match('L', 'L');
    board_temp_ = match('M', 'M');
    redundant_temp_ = match('R', 'R');

    return true;
}


bool PrinterMonitor::parse_capability() {
    auto match = [this](auto& dest, std::string line) -> void {
        auto off = current_line_.find(line);
        if (off != std::string::npos) {
            auto num_str = current_line_.substr(off + line.size() + 1);
            try {
                int res = std::stoi(num_str);
                dest = res;
            } catch (...) {
            }
        }
    };

    auto extract_str_cap = [this](std::string& dest, const std::string& what, const std::string& next) -> void {
        auto off = current_line_.find(what);
        if (off != std::string::npos) {
            auto val = current_line_.substr(off + what.size() + 1);
            dest = val.substr(0, val.find(" " + next));
        }
    };

    extract_str_cap(capabilities_.FIRMWARE_NAME, "FIRMWARE_NAME", "SOURCE_CODE_URL");
    extract_str_cap(capabilities_.SOURCE_CODE_URL, "SOURCE_CODE_URL", "PROTOCOL_VERSION");
    extract_str_cap(capabilities_.PROTOCOL_VERSION, "PROTOCOL_VERSION", "MACHINE_TYPE");
    extract_str_cap(capabilities_.MACHINE_TYPE, "MACHINE_TYPE", "EXTRUDER_COUNT");
    extract_str_cap(capabilities_.UUID, "UUID", "");


#define _FIND_CAP(WHAT) match(capabilities_.WHAT, #WHAT)
    _FIND_CAP(AXIS_COUNT);
    _FIND_CAP(EXTRUDER_COUNT);
    _FIND_CAP(PAREN_COMMENTS);
    _FIND_CAP(GCODE_QUOTED_STRINGS);
    _FIND_CAP(SERIAL_XON_XOFF);
    _FIND_CAP(BINARY_FILE_TRANSFER);
    _FIND_CAP(EEPROM);
    _FIND_CAP(VOLUMETRIC);
    _FIND_CAP(AUTOREPORT_POS);
    _FIND_CAP(AUTOREPORT_TEMP);
    _FIND_CAP(PROGRESS);
    _FIND_CAP(PRINT_JOB);
    _FIND_CAP(AUTOLEVEL);
    _FIND_CAP(RUNOUT);
    _FIND_CAP(Z_PROBE);
    _FIND_CAP(LEVELING_DATA);
    _FIND_CAP(BUILD_PERCENT);
    _FIND_CAP(SOFTWARE_POWER);
    _FIND_CAP(TOGGLE_LIGHTS);
    _FIND_CAP(CASE_LIGHT_BRIGHTNESS);
    _FIND_CAP(SPINDLE);
    _FIND_CAP(LASER);
    _FIND_CAP(EMERGENCY_PARSER);
    _FIND_CAP(HOST_ACTION_COMMANDS);
    _FIND_CAP(PROMPT_SUPPORT);
    _FIND_CAP(SDCARD);
    _FIND_CAP(MULTI_VOLUME);
    _FIND_CAP(REPEAT);
    _FIND_CAP(SD_WRITE);
    _FIND_CAP(AUTOREPORT_SD_STATUS);
    _FIND_CAP(LONG_FILENAME);
    _FIND_CAP(LFN_WRITE);
    _FIND_CAP(CUSTOM_FIRMWARE_UPLOAD);
    _FIND_CAP(EXTENDED_M20);
    _FIND_CAP(THERMAL_PROTECTION);
    _FIND_CAP(MOTION_MODES);
    _FIND_CAP(ARCS);
    _FIND_CAP(BABYSTEPPING);
    _FIND_CAP(CHAMBER_TEMPERATURE);
    _FIND_CAP(COOLER_TEMPERATURE);
    _FIND_CAP(MEATPACK);
    _FIND_CAP(CONFIG_EXPORT);

#undef _FIND_CAP

    return true;
}
