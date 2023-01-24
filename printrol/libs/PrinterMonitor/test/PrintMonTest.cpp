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
    EXPECT_EQ(0, caps.EXTRUDER_COUNT);
    EXPECT_FALSE(caps.PAREN_COMMENTS);
    EXPECT_FALSE(caps.GCODE_QUOTED_STRINGS);
    EXPECT_FALSE(caps.SERIAL_XON_XOFF);
    EXPECT_FALSE(caps.BINARY_FILE_TRANSFER);
    EXPECT_FALSE(caps.EEPROM);
    EXPECT_FALSE(caps.VOLUMETRIC);
    EXPECT_FALSE(caps.AUTOREPORT_POS);
    EXPECT_FALSE(caps.AUTOREPORT_TEMP);
    EXPECT_FALSE(caps.PROGRESS);
    EXPECT_FALSE(caps.PRINT_JOB);
    EXPECT_FALSE(caps.AUTOLEVEL);
    EXPECT_FALSE(caps.RUNOUT);
    EXPECT_FALSE(caps.Z_PROBE);
    EXPECT_FALSE(caps.LEVELING_DATA);
    EXPECT_FALSE(caps.BUILD_PERCENT);
    EXPECT_FALSE(caps.SOFTWARE_POWER);
    EXPECT_FALSE(caps.TOGGLE_LIGHTS);
    EXPECT_FALSE(caps.CASE_LIGHT_BRIGHTNESS);
    EXPECT_FALSE(caps.SPINDLE);
    EXPECT_FALSE(caps.LASER);
    EXPECT_FALSE(caps.EMERGENCY_PARSER);
    EXPECT_FALSE(caps.HOST_ACTION_COMMANDS);
    EXPECT_FALSE(caps.PROMPT_SUPPORT);
    EXPECT_FALSE(caps.SDCARD);
    EXPECT_FALSE(caps.MULTI_VOLUME);
    EXPECT_FALSE(caps.REPEAT);
    EXPECT_FALSE(caps.SD_WRITE);
    EXPECT_FALSE(caps.AUTOREPORT_SD_STATUS);
    EXPECT_FALSE(caps.LONG_FILENAME);
    EXPECT_FALSE(caps.LFN_WRITE);
    EXPECT_FALSE(caps.CUSTOM_FIRMWARE_UPLOAD);
    EXPECT_FALSE(caps.EXTENDED_M20);
    EXPECT_FALSE(caps.THERMAL_PROTECTION);
    EXPECT_FALSE(caps.MOTION_MODES);
    EXPECT_FALSE(caps.ARCS);
    EXPECT_FALSE(caps.BABYSTEPPING);
    EXPECT_FALSE(caps.CHAMBER_TEMPERATURE);
    EXPECT_FALSE(caps.COOLER_TEMPERATURE);
    EXPECT_FALSE(caps.MEATPACK);
    EXPECT_FALSE(caps.CONFIG_EXPORT);
}


struct PrinterMonitorTestCapabilities : public ::testing::Test {
protected:
    PrinterMonitor mon;

    void SetUp() override {
        std::vector<std::string> lines = {
            "FIRMWARE_NAME:Marlin bugfix-2.1.x (Jan 23 2023 23:25:27) SOURCE_CODE_URL:github.com/MarlinFirmware/Marlin "
            "PROTOCOL_VERSION:1.0 MACHINE_TYPE:3D Printer EXTRUDER_COUNT:1 AXIS_COUNT:5 "
            "UUID:cede2a2f-41a2-4748-9b12-c55c62f367ff",
            "Cap:PAREN_COMMENTS:1",
            "Cap:GCODE_QUOTED_STRINGS:1",
            "Cap:SERIAL_XON_XOFF:1",
            "Cap:BINARY_FILE_TRANSFER:1",
            "Cap:EEPROM:1",
            "Cap:VOLUMETRIC:1",
            "Cap:AUTOREPORT_POS:1",
            "Cap:AUTOREPORT_TEMP:1",
            "Cap:PROGRESS:1",
            "Cap:PRINT_JOB:1",
            "Cap:AUTOLEVEL:1",
            "Cap:RUNOUT:1",
            "Cap:Z_PROBE:1",
            "Cap:LEVELING_DATA:1",
            "Cap:BUILD_PERCENT:1",
            "Cap:SOFTWARE_POWER:1",
            "Cap:TOGGLE_LIGHTS:1",
            "Cap:CASE_LIGHT_BRIGHTNESS:1",
            "Cap:SPINDLE:1",
            "Cap:LASER:1",
            "Cap:EMERGENCY_PARSER:1",
            "Cap:HOST_ACTION_COMMANDS:1",
            "Cap:PROMPT_SUPPORT:1",
            "Cap:SDCARD:1",
            "Cap:MULTI_VOLUME:1",
            "Cap:REPEAT:1",
            "Cap:SD_WRITE:1",
            "Cap:AUTOREPORT_SD_STATUS:1",
            "Cap:LONG_FILENAME:1",
            "Cap:LFN_WRITE:1",
            "Cap:CUSTOM_FIRMWARE_UPLOAD:1",
            "Cap:EXTENDED_M20:1",
            "Cap:THERMAL_PROTECTION:1",
            "Cap:MOTION_MODES:1",
            "Cap:ARCS:1",
            "Cap:BABYSTEPPING:1",
            "Cap:CHAMBER_TEMPERATURE:1",
            "Cap:COOLER_TEMPERATURE:1",
            "Cap:MEATPACK:1",
            "Cap:CONFIG_EXPORT:1",
        };
    }
};

TEST_F(PrinterMonitorTestCapabilities, TestAllCapabilities) {
    const auto caps = mon.get_capabilities();

    EXPECT_EQ(std::string("Marlin bugfix-2.1.x (Jan 23 2023 23:25:27)"), caps.FIRMWARE_NAME);
    EXPECT_EQ(std::string("github.com/MarlinFirmware/Marlin"), caps.SOURCE_CODE_URL);
    EXPECT_EQ(std::string("1.0"), caps.PROTOCOL_VERSION);
    EXPECT_EQ(std::string("3D Printer"), caps.MACHINE_TYPE);
    EXPECT_EQ(std::string("cede2a2f-41a2-4748-9b12-c55c62f367ff"), caps.UUID);
    EXPECT_EQ(5, caps.AXIS_COUNT);
    EXPECT_EQ(1, caps.EXTRUDER_COUNT);
    EXPECT_TRUE(caps.PAREN_COMMENTS);
    EXPECT_TRUE(caps.GCODE_QUOTED_STRINGS);
    EXPECT_TRUE(caps.SERIAL_XON_XOFF);
    EXPECT_TRUE(caps.BINARY_FILE_TRANSFER);
    EXPECT_TRUE(caps.EEPROM);
    EXPECT_TRUE(caps.VOLUMETRIC);
    EXPECT_TRUE(caps.AUTOREPORT_POS);
    EXPECT_TRUE(caps.AUTOREPORT_TEMP);
    EXPECT_TRUE(caps.PROGRESS);
    EXPECT_TRUE(caps.PRINT_JOB);
    EXPECT_TRUE(caps.AUTOLEVEL);
    EXPECT_TRUE(caps.RUNOUT);
    EXPECT_TRUE(caps.Z_PROBE);
    EXPECT_TRUE(caps.LEVELING_DATA);
    EXPECT_TRUE(caps.BUILD_PERCENT);
    EXPECT_TRUE(caps.SOFTWARE_POWER);
    EXPECT_TRUE(caps.TOGGLE_LIGHTS);
    EXPECT_TRUE(caps.CASE_LIGHT_BRIGHTNESS);
    EXPECT_TRUE(caps.SPINDLE);
    EXPECT_TRUE(caps.LASER);
    EXPECT_TRUE(caps.EMERGENCY_PARSER);
    EXPECT_TRUE(caps.HOST_ACTION_COMMANDS);
    EXPECT_TRUE(caps.PROMPT_SUPPORT);
    EXPECT_TRUE(caps.SDCARD);
    EXPECT_TRUE(caps.MULTI_VOLUME);
    EXPECT_TRUE(caps.REPEAT);
    EXPECT_TRUE(caps.SD_WRITE);
    EXPECT_TRUE(caps.AUTOREPORT_SD_STATUS);
    EXPECT_TRUE(caps.LONG_FILENAME);
    EXPECT_TRUE(caps.LFN_WRITE);
    EXPECT_TRUE(caps.CUSTOM_FIRMWARE_UPLOAD);
    EXPECT_TRUE(caps.EXTENDED_M20);
    EXPECT_TRUE(caps.THERMAL_PROTECTION);
    EXPECT_TRUE(caps.MOTION_MODES);
    EXPECT_TRUE(caps.ARCS);
    EXPECT_TRUE(caps.BABYSTEPPING);
    EXPECT_TRUE(caps.CHAMBER_TEMPERATURE);
    EXPECT_TRUE(caps.COOLER_TEMPERATURE);
    EXPECT_TRUE(caps.MEATPACK);
    EXPECT_TRUE(caps.CONFIG_EXPORT);
}
