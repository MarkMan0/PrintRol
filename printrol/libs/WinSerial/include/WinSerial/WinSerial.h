#pragma once

#include <string_view>
#include <Windows.h>

class WinSerial {
public:
    void open(int port, int baud);

    int write(const void* buff, int size);
    int read(void* dest, int size);

    void flush();

    bool is_open() const;

    void close();

    ~WinSerial();

private:
    HANDLE com_handle_{ INVALID_HANDLE_VALUE };
    COMMTIMEOUTS timeouts_old_, timeouts_new_;
    DCB dcb_old_, dcb_new_;
};
