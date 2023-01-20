#ifndef PRINTROLWINDOW_H
#define PRINTROLWINDOW_H

#include <QMainWindow>
#include <WinSerial/WinSerial.h>
#include "CommThread.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class PrintRolWindow;
}
QT_END_NAMESPACE

class PrintRolWindow : public QMainWindow {
    Q_OBJECT

public:
    PrintRolWindow(QWidget* parent = nullptr);
    ~PrintRolWindow();

private slots:

    void refresh_ports();

    void connect_to_port();

    void disconnect_port();

    void update_port_label();

    void user_txt_input();

    void send_to_printer(const QString& str);

    void byte_received(std::uint8_t);

private:
    Ui::PrintRolWindow* ui;
    WinSerial serial_;
    CommThread comm_thrd_;
};
#endif  // PRINTROLWINDOW_H
