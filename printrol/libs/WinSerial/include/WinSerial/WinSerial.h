#pragma once

#include <string_view>
#include <Windows.h>
#include <ISerial/ISerial.h>

class WinSerial final : public ISerial {
public:
    void open(std::wstring port, int baud) override;

    int write(const void* buff, int size) override;
    int read(void* dest, int size) override;

    void flush() override;

    bool is_open() const override;

    void close() override;

    virtual ~WinSerial();

    std::vector<std::wstring> list_ports() const override;

private:
    HANDLE com_handle_{ INVALID_HANDLE_VALUE };
    COMMTIMEOUTS timeouts_old_, timeouts_new_;
    DCB dcb_old_, dcb_new_;
};
