#pragma once

#include <QThread>
#include <QAtomicInt>
#include <ISerial/ISerial.h>
#include <PrinterMonitor/PrinterMonitor.h>
#include <string>


class CommThread : public QThread {
    Q_OBJECT

public:
    CommThread() = default;
    ~CommThread() {
        abort_ = 1;
        while (running_) {
        }
        QThread::msleep(10);
    }

    void set_serial(ISerial* serial) {
        serial_ = serial;
    }

    PrinterMonitor& get_printer() {
        return mon_;
    }
    const PrinterMonitor& get_printer() const {
        return mon_;
    }

signals:
    void line_received(std::string);
    void printer_status_changed();

protected:
    void run() override {
        running_ = 1;
        inner_run();
        running_ = 0;
    }

private:
    void inner_run() {
        if (serial_ == nullptr) {
            return;
        }


        forever {
            std::uint8_t b;

            if (abort_) {
                return;
            }

            if (serial_->read(&b, 1) == 1) {
                line_buffer_ += static_cast<char>(b);
                if (b == '\n') {
                    emit line_received(line_buffer_);
                    if (mon_.parse_line(line_buffer_)) {
                        emit printer_status_changed();
                    }
                    line_buffer_.clear();
                }
            } else {
                QThread::msleep(100);
            }
        }
    }

    std::string line_buffer_;
    ISerial* serial_{ nullptr };
    QAtomicInt abort_{ 0 }, running_{ 0 };
    PrinterMonitor mon_;
};
