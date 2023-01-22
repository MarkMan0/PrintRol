#include "PrinterMonitor/PrinterMonitor.h"
#include <regex>
#include <array>

void PrinterMonitor::reset() {
    position_.clear();
    position_known_ = false;
    has_leveling_ = false;
    leveling_active_ = false;

    hotend_temps_.clear();
    bed_temp_.clear();
    chamber_temp_.clear();
}

void PrinterMonitor::parse_line(std::string_view line) {
    current_line_ = line;
    parse_position();
}


void PrinterMonitor::parse_position() {
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
}
