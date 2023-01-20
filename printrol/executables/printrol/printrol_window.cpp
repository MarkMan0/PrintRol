#include "printrol_window.h"
#include "./ui_printrol_window.h"

#include <vector>
#include <string>
#include <ComList/ComList.h>

PrintRolWindow::PrintRolWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::PrintRolWindow) {
    ui->setupUi(this);

    connect(ui->portRefreshButton, &QPushButton::clicked, this, &PrintRolWindow::refresh_ports);
    connect(ui->portConnectButton, &QPushButton::clicked, this, &PrintRolWindow::connect_to_port);
    connect(ui->portDisconnectButton, &QPushButton::clicked, this, &PrintRolWindow::disconnect_port);
    connect(ui->inputTextEdit, &QTextEdit::textChanged, this, &PrintRolWindow::user_txt_input);
    connect(&comm_thrd_, &CommThread::byte_received, this, &PrintRolWindow::byte_received);

    comm_thrd_.set_serial(&serial_);
    comm_thrd_.start(QThread::NormalPriority);

    refresh_ports();
    update_port_label();
}

PrintRolWindow::~PrintRolWindow() {
    delete ui;
}

void PrintRolWindow::refresh_ports() {
    // TODO: get list of actual ports
    auto ports = get_com_ports();

    ui->portSelectBox->clear();
    for (const auto& port : ports) {
        auto qstr = QString::fromStdWString(port.port_str_);
        qstr.remove(0, 4);
        ui->portSelectBox->addItem(qstr);
    }
}


void PrintRolWindow::connect_to_port() {
    auto current_port = ui->portSelectBox->currentText();
    if (current_port.length() < 4) {
        //  not valid COMx text
        return;
    }
    current_port.remove(0, 3);
    int port_no = current_port.toInt();

    serial_.open(port_no, 115200);
    update_port_label();
}

void PrintRolWindow::disconnect_port() {
    serial_.close();
    update_port_label();
}


void PrintRolWindow::update_port_label() {
    ui->portStatusLabel->setText(serial_.is_open() ? "Connected" : "Disconnected");
}


void PrintRolWindow::user_txt_input() {
    auto txt = ui->inputTextEdit->toPlainText();

    if (txt.length() > 0 && (txt.back() == '\n' || txt.back() == '\r')) {
        send_to_printer(txt);
        ui->inputTextEdit->clear();
    }
}

void PrintRolWindow::send_to_printer(const QString& qstr) {
    if (not serial_.is_open()) {
        return;
    }
    std::string stdstr = qstr.toStdString();

    serial_.write(stdstr.c_str(), stdstr.length());
}

void PrintRolWindow::byte_received(std::uint8_t b) {
    char c = static_cast<char>(b);
    QString str;
    str += c;
    ui->historyTextEdit->insertPlainText(str);
}
