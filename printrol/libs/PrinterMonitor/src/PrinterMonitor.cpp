#include "PrinterMonitor/PrinterMonitor.h"
#include <regex>
#include <array>
#include <tuple>

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
    parse_position();
    parse_temperature();
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


void PrinterMonitor::parse_temperature() {
    const std::string num_matcher = "(\\d*.?\\d*) \\/(\\d*.?\\d*)";

    // matcher
    auto match = [&num_matcher, this](char prefix_1, char prefix_2) -> temp_t {
        std::smatch m;
        std::string actual, target, power;
        std::string rgx1;
        rgx1 += prefix_1;
        rgx1 += ":" + num_matcher;
        std::string rgx2;
        rgx2 += prefix_2;
        rgx2 += "@:(\\d*)";
        if (std::regex_search(current_line_, m, std::regex(rgx1)) && m.size() > 2) {
            actual = m[1];
            target = m[2];
            if (std::regex_search(current_line_, m, std::regex(rgx2)) && m.size() > 1) {
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

    // match hotend (T)
    hotend_temps_.push_back(match('T', ' '));
    bed_temp_ = match('B', 'B');
    chamber_temp_ = match('C', 'C');
    probe_temp_ = match('P', 'P');
    cooler_temp_ = match('L', 'L');
    board_temp_ = match('M', 'M');
    redundant_temp_ = match('R', 'R');
}
