#include <gtest/gtest.h>
#include "LineFilter/LineFilter.h"


TEST(LineFilterTest, TestTemperatureMatch) {
    const std::regex rgx = LineFilter::temperature_regex();
    const std::string s1(" T:21.56 /0.00 B:22.34 /0.00 @:0 B@:0\n"),
        s2("T0:112.23 /140.00 T1:268.45 B:22.34 /0.00 @:0 B@:0"), s3("T:something random");

    EXPECT_TRUE(std::regex_search(s1, rgx));
    EXPECT_TRUE(std::regex_search(s2, rgx));
    EXPECT_FALSE(std::regex_search(s3, rgx));
}

TEST(LineFilterTest, TestPositionMatch) {
    const std::regex rgx = LineFilter::position_regex();
    const std::string s1("X:-5.00 Y:-17.00 Z:0.00 E:0.00 Count X:-400 Y:-1360 Z:0"),
        s2("X:5.00 Y:0.00 Z:130.00 E:0.00 Count X:-400 Y:-1360 Z:0"), s3("X:ok");

    EXPECT_TRUE(std::regex_search(s1, rgx));
    EXPECT_TRUE(std::regex_search(s2, rgx));
    EXPECT_FALSE(std::regex_search(s3, rgx));
}

TEST(LineFilterTest, TestCheck) {
    LineFilter filter;
    filter.add_filter(LineFilter::temperature_regex());
    filter.add_filter(LineFilter::position_regex());

    EXPECT_FALSE(filter.check(" T:21.56 /0.00 B:22.34 /0.00 @:0 B@:0\n"));
    EXPECT_FALSE(filter.check("T0:112.23 /140.00 T1:268.45 B:22.34 /0.00 @:0 B@:0"));
    EXPECT_FALSE(filter.check("X:-5.00 Y:-17.00 Z:0.00 E:0.00 Count X:-400 Y:-1360 Z:0"));
    EXPECT_FALSE(filter.check("X:5.00 Y:0.00 Z:130.00 E:0.00 Count X:-400 Y:-1360 Z:0\n"));
    EXPECT_TRUE(filter.check("ok"));
    EXPECT_TRUE("SD OK\n");
}
