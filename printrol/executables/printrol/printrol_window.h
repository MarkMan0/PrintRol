#ifndef PRINTROLWINDOW_H
#define PRINTROLWINDOW_H

#include <QMainWindow>
#include <ISerial/ISerial.h>
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

    void init();

    void set_serial(ISerial* serial) {
        serial_ = serial;
    }

private slots:

    void refresh_ports();
    void connect_to_port();
    void disconnect_port();
    void update_port_label();
    void user_txt_input();
    void send_to_printer(const QString& str);
    void line_received(std::string);
    void enter_on_combobox();
    void printer_status_change();

private:
    Ui::PrintRolWindow* ui;
    ISerial* serial_;
    CommThread comm_thrd_;
};
#endif  // PRINTROLWINDOW_H
