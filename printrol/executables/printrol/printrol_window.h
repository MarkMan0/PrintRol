#ifndef PRINTROLWINDOW_H
#define PRINTROLWINDOW_H

#include <QMainWindow>
#include <WinSerial/WinSerial.h>

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

private:
    Ui::PrintRolWindow* ui;
    WinSerial serial_;
};
#endif  // PRINTROLWINDOW_H
