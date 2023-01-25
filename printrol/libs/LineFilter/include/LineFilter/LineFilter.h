#pragma once
#include <regex>
#include <vector>


class LineFilter {
public:
    bool check(const std::string& line);

    void add_filter(const std::regex& filter);

private:
    std::vector<std::regex> filters_;

public:
    static std::regex temperature_regex();
    static std::regex position_regex();
};
