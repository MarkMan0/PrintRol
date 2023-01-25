#include "LineFilter/LineFilter.h"


bool LineFilter::check(const std::string& line) {
    for (const auto& rgx : filters_) {
        if (std::regex_search(line, rgx)) {
            return false;
        }
    }
    return true;
}

void LineFilter::add_filter(const std::regex& filter) {
    filters_.push_back(filter);
}


std::regex LineFilter::temperature_regex() {
    // for reference: \s?T\d?:\d+.\d+ \/\d+.\d+.+?(?=@)
    // T\d?: - match T: or T0:
    // \d+.\d+ - match number in form xx.xx
    // \/ - match '/'
    // \d+.\d+ - see above
    // .+?(?=@) - match everything until '@' is found
    return std::regex("\\s?T\\d?:\\d+.\\d+ \\/\\d+.\\d+.+?(?=@)");
}

std::regex LineFilter::position_regex() {
    std::string num_match = "-?\\d+.\\d+";
    return std::regex("\\s?X:" + num_match + " Y:" + num_match + " Z:" + num_match);
}
