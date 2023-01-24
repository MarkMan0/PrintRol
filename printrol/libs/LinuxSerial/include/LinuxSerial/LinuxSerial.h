#pragma once

#include <ISerial/ISerial.h>

class LinuxSerial final : public ISerial {
public:
    void open(std::wstring port, int baud) override;

    int write(const void* buff, int size) override;
    int read(void* dest, int size) override;

    void flush() override;

    bool is_open() const override;

    void close() override;

    virtual ~LinuxSerial();

    std::vector<std::wstring> list_ports() const override;

private:
    int port_handle_{ -1 };
};
