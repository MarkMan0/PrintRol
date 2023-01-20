#pragma once

#include <QThread>
#include <QAtomicInt>
#include <ISerial/ISerial.h>

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

signals:
    void byte_received(std::uint8_t);

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
                emit byte_received(b);
            } else {
                QThread::msleep(100);
            }
        }
    }

    ISerial* serial_{ nullptr };
    QAtomicInt abort_{ 0 }, running_{ 0 };
};
