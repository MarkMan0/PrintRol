#include <gtest/gtest.h>
#include "PrinterMonitor/PrinterMonitor.h"

TEST(PrinterMonitorTest, ParsePosition) {
    PrinterMonitor mon;
    std::vector<float> res, expected;
    auto verify = [&res, &expected]() {
        EXPECT_EQ(expected.size(), res.size());

        for (int i = 0; i < expected.size(); ++i) {
            EXPECT_FLOAT_EQ(expected[i], res[i]);
        }
    };

    mon.parse_line("X:0.00 Y:127.00 Z:145.00 E:0.00 Count X: 0 Y:10160 Z:116000");
    res = mon.get_position();
    expected = { 0.0, 127.0, 145.0, 0.0 };
    verify();

    mon.parse_line("X:-26.4 Y:0.9999 Z:-0.0 E:3.14 Count X: 0 Y:10160 Z:116000");
    res = mon.get_position();
    expected = { -26.4, 0.9999, 0, 3.14 };
    verify();

    // wrong data - expect no change
    mon.parse_line("X:a-26.4 Y:0.a9999 Z:-0.0 E:3.14 Count X: 0 Y:10160 Z:116000");
    res = mon.get_position();
    verify();

    mon.parse_line("echo: hello world");
    res = mon.get_position();
    verify();

    mon.parse_line("");
    res = mon.get_position();
    verify();
}
