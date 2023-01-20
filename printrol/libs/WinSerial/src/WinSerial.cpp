#include "WinSerial/WinSerial.h"
#include <string>

void WinSerial::open(int port, int baud) {
    if (is_open()) {
        close();
    }

    std::wstring port_str(L"//.////COM");
    port_str += std::to_wstring(port);
    com_handle_ = CreateFile(port_str.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (com_handle_ == INVALID_HANDLE_VALUE) return;

    GetCommTimeouts(com_handle_, &timeouts_old_);

    timeouts_new_ = timeouts_old_;

    timeouts_new_.ReadIntervalTimeout = MAXDWORD;
    timeouts_new_.ReadTotalTimeoutConstant = 0;
    timeouts_new_.ReadTotalTimeoutMultiplier = 0;
    timeouts_new_.WriteTotalTimeoutMultiplier = 0;
    timeouts_new_.WriteTotalTimeoutConstant = 0;

    SetCommTimeouts(com_handle_, &timeouts_new_);

    GetCommState(com_handle_, &dcb_old_);

    dcb_new_ = dcb_old_;

    dcb_new_.BaudRate = baud;
    dcb_new_.ByteSize = 8;
    dcb_new_.fParity = 0;
    dcb_new_.Parity = 0;
    dcb_new_.StopBits = 0;

    SetCommState(com_handle_, &dcb_new_);
}

int WinSerial::write(const void* buff, int size) {
    if (not is_open()) {
        return 0;
    }
    DWORD cnt = 0;
    WriteFile(com_handle_, buff, size, &cnt, NULL);
    return cnt;
}

int WinSerial::read(void* dst, int sz) {
    if (not is_open()) {
        return 0;
    }
    DWORD cnt = 0;
    ReadFile(com_handle_, dst, sz, &cnt, NULL);
    return cnt;
}

void WinSerial::flush() {
    if (not is_open()) {
        return;
    }
    FlushFileBuffers(com_handle_);
}

bool WinSerial::is_open() const {
    return com_handle_ != INVALID_HANDLE_VALUE;
}

void WinSerial::close() {
    if (not is_open()) {
        return;
    }
    SetCommTimeouts(com_handle_, &timeouts_old_);
    SetCommState(com_handle_, &dcb_old_);
    CloseHandle(com_handle_);
    com_handle_ = INVALID_HANDLE_VALUE;
}

WinSerial::~WinSerial() {
    close();
}
