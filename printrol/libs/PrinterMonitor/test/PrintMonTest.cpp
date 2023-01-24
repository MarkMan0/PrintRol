#include <gtest/gtest.h>
#include "PrinterMonitor/PrinterMonitor.h"



TEST(PrinterMonitorTest, ParsePosition) {
    PrinterMonitor mon;
    std::vector<float> res, expected;

    mon.parse_line("X:0.00 Y:127.00 Z:145.00 E:0.00 Count X: 0 Y:10160 Z:116000");
    res = mon.get_position();
    expected = { 0.0, 127.0, 145.0, 0.0 };
    EXPECT_EQ(res, expected);

    mon.parse_line("X:-26.4 Y:0.9999 Z:-0.0 E:3.14 Count X: 0 Y:10160 Z:116000");
    res = mon.get_position();
    expected = { -26.4, 0.9999, 0, 3.14 };
    EXPECT_EQ(res, expected);

    // wrong data - expect no change
    mon.parse_line("X:a-26.4 Y:0.a9999 Z:-0.0 E:3.14 Count X: 0 Y:10160 Z:116000");
    res = mon.get_position();
    EXPECT_EQ(res, expected);

    mon.parse_line("echo: hello world");
    res = mon.get_position();
    EXPECT_EQ(res, expected);

    mon.parse_line("");
    res = mon.get_position();
    EXPECT_EQ(res, expected);
}


struct PrinterMonitorTestTemperature : public ::testing::Test {
protected:
    PrinterMonitor mon;
    std::string get_single_extruder_line() const {
        return " T:214.32 /220.0"
               " B:46.54 /80.00"
               " C:24.34 /31.13"
               " P:20.41 /25.14"
               " L:36.00 /14.14"
               " M:26.34 /0.00"
               " R:86.2 /220.00"
               " @:114 B@:26 C@:127 C@:54";
    }
    PrinterMonitor::temp_t expected;

    void SetUp() override {
        mon.parse_line(get_single_extruder_line());
    }
};

TEST_F(PrinterMonitorTestTemperature, TestSingleHotend) {
    expected = { 214.32, 220.0, 114 };
    EXPECT_EQ(expected, mon.get_hotend_temp());
    EXPECT_EQ(PrinterMonitor::temp_t{}, mon.get_hotend_temp(1));
}

TEST_F(PrinterMonitorTestTemperature, TestBed) {
    expected = { 46.54, 80.0, 26 };
    EXPECT_EQ(expected, mon.get_bed_temp());
}

TEST_F(PrinterMonitorTestTemperature, TestChamber) {
    expected = { 24.34, 31.13, 127 };
    EXPECT_EQ(expected, mon.get_chamber_temp());
}

TEST_F(PrinterMonitorTestTemperature, TestProbe) {
    expected = { 20.41, 25.14, 0.00 };
    EXPECT_EQ(expected, mon.get_probe_temp());
}

TEST_F(PrinterMonitorTestTemperature, TestCooler) {
    expected = { 36.00, 14.14, 0 };
    EXPECT_EQ(expected, mon.get_cooler_temp());
}

TEST_F(PrinterMonitorTestTemperature, TestBoard) {
    expected = { 26.34, 0.00, 0.00 };
    EXPECT_EQ(expected, mon.get_board_temp());
}

TEST_F(PrinterMonitorTestTemperature, TestRedundant) {
    expected = { 86.2, 220.00, 0.00 };
    EXPECT_EQ(expected, mon.get_redundant_temp());
}

TEST_F(PrinterMonitorTestTemperature, TestReset) {
    mon.reset();
    std::vector<float> empty;

    EXPECT_EQ(empty, mon.get_position());
    EXPECT_FALSE(mon.position_known());
    EXPECT_FALSE(mon.has_leveling());
    EXPECT_FALSE(mon.is_leveling_active());

    EXPECT_EQ(empty, mon.get_hotend_temp());
    EXPECT_EQ(empty, mon.get_bed_temp());
    EXPECT_EQ(empty, mon.get_chamber_temp());
    EXPECT_EQ(empty, mon.get_probe_temp());
    EXPECT_EQ(empty, mon.get_cooler_temp());
    EXPECT_EQ(empty, mon.get_board_temp());
    EXPECT_EQ(empty, mon.get_redundant_temp());
}

TEST_F(PrinterMonitorTestTemperature, TestMultiHotend) {
    mon.parse_line(" T0:112.23 /140.00 T1:268.45 /320.12 T2:20.12 /0.00 @0:14 @1:65 @2:0");

    expected = { 112.23, 140, 14 };
    EXPECT_EQ(expected, mon.get_hotend_temp(0));
    expected = { 268.45, 320.12, 65 };
    EXPECT_EQ(expected, mon.get_hotend_temp(1));
    expected = { 20.12, 0, 0 };
    EXPECT_EQ(expected, mon.get_hotend_temp(2));
}

TEST(PrinterMonitorTest, TestCapabilitiesDefaults) {
    PrinterMonitor mon;
    auto caps = mon.get_capabilities();

    EXPECT_EQ(std::string(), caps.FIRMWARE_NAME);
    EXPECT_EQ(std::string(), caps.SOURCE_CODE_URL);
    EXPECT_EQ(std::string(), caps.PROTOCOL_VERSION);
    EXPECT_EQ(std::string(), caps.MACHINE_TYPE);
    EXPECT_EQ(std::string(), caps.UUID);
    EXPECT_EQ(3, caps.AXIS_COUNT);
    EXPECT_EQ(false, caps.EXTRUDER_COUNT);
    EXPECT_EQ(false, caps.PAREN_COMMENTS);
    EXPECT_EQ(false, caps.GCODE_QUOTED_STRINGS);
    EXPECT_EQ(false, caps.SERIAL_XON_XOFF);
    EXPECT_EQ(false, caps.BINARY_FILE_TRANSFER);
    EXPECT_EQ(false, caps.EEPROM);
    EXPECT_EQ(false, caps.VOLUMETRIC);
    EXPECT_EQ(false, caps.AUTOREPORT_POS);
    EXPECT_EQ(false, caps.AUTOREPORT_TEMP);
    EXPECT_EQ(false, caps.PROGRESS);
    EXPECT_EQ(false, caps.PRINT_JOB);
    EXPECT_EQ(false, caps.AUTOLEVEL);
    EXPECT_EQ(false, caps.RUNOUT);
    EXPECT_EQ(false, caps.Z_PROBE);
    EXPECT_EQ(false, caps.LEVELING_DATA);
    EXPECT_EQ(false, caps.BUILD_PERCENT);
    EXPECT_EQ(false, caps.SOFTWARE_POWER);
    EXPECT_EQ(false, caps.TOGGLE_LIGHTS);
    EXPECT_EQ(false, caps.CASE_LIGHT_BRIGHTNESS);
    EXPECT_EQ(false, caps.SPINDLE);
    EXPECT_EQ(false, caps.LASER);
    EXPECT_EQ(false, caps.EMERGENCY_PARSER);
    EXPECT_EQ(false, caps.HOST_ACTION_COMMANDS);
    EXPECT_EQ(false, caps.PROMPT_SUPPORT);
    EXPECT_EQ(false, caps.SDCARD);
    EXPECT_EQ(false, caps.MULTI_VOLUME);
    EXPECT_EQ(false, caps.REPEAT);
    EXPECT_EQ(false, caps.SD_WRITE);
    EXPECT_EQ(false, caps.AUTOREPORT_SD_STATUS);
    EXPECT_EQ(false, caps.LONG_FILENAME);
    EXPECT_EQ(false, caps.LFN_WRITE);
    EXPECT_EQ(false, caps.CUSTOM_FIRMWARE_UPLOAD);
    EXPECT_EQ(false, caps.EXTENDED_M20);
    EXPECT_EQ(false, caps.THERMAL_PROTECTION);
    EXPECT_EQ(false, caps.MOTION_MODES);
    EXPECT_EQ(false, caps.ARCS);
    EXPECT_EQ(false, caps.BABYSTEPPING);
    EXPECT_EQ(false, caps.CHAMBER_TEMPERATURE);
    EXPECT_EQ(false, caps.COOLER_TEMPERATURE);
    EXPECT_EQ(false, caps.MEATPACK);
    EXPECT_EQ(false, caps.CONFIG_EXPORT);
}