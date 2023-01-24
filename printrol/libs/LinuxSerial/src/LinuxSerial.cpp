#include "LinuxSerial/LinuxSerial.h"

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include <codecvt>
#include <locale>


static constexpr int baud_to_enum(int baud) {
    switch (baud) {
        case 50:
            return B50;
        case 75:
            return B75;
        case 110:
            return B110;
        case 134:
            return B134;
        case 150:
            return B150;
        case 200:
            return B200;
        case 300:
            return B300;
        case 600:
            return B600;
        case 1200:
            return B1200;
        case 1800:
            return B1800;
        case 2400:
            return B2400;
        case 4800:
            return B4800;
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        case 500000:
            return B500000;
        case 576000:
            return B576000;
        case 921600:
            return B921600;
        case 1000000:
            return B1000000;
        case 1152000:
            return B1152000;
        case 1500000:
            return B1500000;
        case 2000000:
            return B2000000;
        case 2500000:
            return B2500000;
        case 3000000:
            return B3000000;
        case 3500000:
            return B3500000;
        case 4000000:
            return B4000000;
        default:
            return baud;
    }
}


void LinuxSerial::open(std::wstring port, int baud) {
    std::string port_str = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(port);

    port_handle_ = ::open(port_str.c_str(), O_RDWR);

    if (not is_open()) {
        return;
    }

    struct termios tty;
    if (tcgetattr(port_handle_, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return;
    }

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;    // Disable echo
    tty.c_lflag &= ~ECHOE;   // Disable erasure
    tty.c_lflag &= ~ECHONL;  // Disable new-line echo

    tty.c_lflag &= ~ISIG;  // Disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // Turn off s/w flow ctrl
    tty.c_iflag &=
        ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);  // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST;  // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR;  // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;

    cfsetspeed(&tty, baud_to_enum(baud));

    if (tcsetattr(port_handle_, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }
}


int LinuxSerial::write(const void* buff, int size) {
    return ::write(port_handle_, buff, size);
}

int LinuxSerial::read(void* dest, int size) {
    return ::read(port_handle_, dest, size);
}

void LinuxSerial::flush() {
    return;
}

bool LinuxSerial::is_open() const {
    return port_handle_ >= 0;
}

void LinuxSerial::close() {
    if (is_open()) {
        ::close(port_handle_);
        port_handle_ = -1;
    }
}

LinuxSerial::~LinuxSerial() {
    close();
}

std::vector<std::wstring> LinuxSerial::list_ports() const {
    return { L"/dev/ttyACM0" };
}
