#pragma once
#include <vector>
#include <string>


class ISerial {
public:
    virtual ~ISerial() = default;

    /// @brief Open a serial port with baud
    /// @param port The name without any prefix e.g. COMx, ttyxxx ...
    /// @param baud Requested baud rate
    virtual void open(std::wstring port, int baud) = 0;
    virtual void close() = 0;
    virtual int write(const void* buff, int size) = 0;
    virtual int read(void* dest, int size) = 0;
    virtual void flush() = 0;
    virtual bool is_open() const = 0;

    virtual std::vector<std::wstring> list_ports() const = 0;
};
