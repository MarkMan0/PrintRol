#include "printrol_window.h"
#include "./ui_printrol_window.h"

#include <vector>
#include <string>

PrintRolWindow::PrintRolWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::PrintRolWindow) {
    ui->setupUi(this);

    connect(ui->portRefreshButton, &QPushButton::clicked, this, &PrintRolWindow::refresh_ports);
    connect(ui->portConnectButton, &QPushButton::clicked, this, &PrintRolWindow::connect_to_port);
    connect(ui->portDisconnectButton, &QPushButton::clicked, this, &PrintRolWindow::disconnect_port);
    connect(ui->inputLineEdit, &QLineEdit::returnPressed, this, &PrintRolWindow::user_txt_input);
    connect(&comm_thrd_, &CommThread::byte_received, this, &PrintRolWindow::byte_received);
    connect(ui->portSelectBox->lineEdit(), &QLineEdit::returnPressed, this, &PrintRolWindow::enter_on_combobox);
}

void PrintRolWindow::init() {
    comm_thrd_.set_serial(serial_);
    comm_thrd_.start(QThread::NormalPriority);

    refresh_ports();
    update_port_label();
}

PrintRolWindow::~PrintRolWindow() {
    delete ui;
}

void PrintRolWindow::refresh_ports() {
    auto ports = serial_->list_ports();

    ui->portSelectBox->clear();
    for (const auto& port : ports) {
        auto qstr = QString::fromStdWString(port);
        ui->portSelectBox->addItem(qstr);
    }
}


void PrintRolWindow::connect_to_port() {
    auto current_port = ui->portSelectBox->currentText();
    if (current_port.length() < 1) {
        //  not valid COMx text
        return;
    }

    serial_->open(current_port.toStdWString(), 115200);
    update_port_label();
}

void PrintRolWindow::disconnect_port() {
    serial_->close();
    update_port_label();
}


void PrintRolWindow::update_port_label() {
    ui->portStatusLabel->setText(serial_->is_open() ? "Connected" : "Disconnected");
}


void PrintRolWindow::user_txt_input() {
    auto txt = ui->inputLineEdit->text();

    if (txt.length() > 0) {
        txt += '\n';
        send_to_printer(txt);
        ui->inputLineEdit->clear();
    }
}

void PrintRolWindow::send_to_printer(const QString& qstr) {
    if (not serial_->is_open()) {
        return;
    }
    std::string stdstr = qstr.toStdString();

    serial_->write(stdstr.c_str(), stdstr.length());
}

void PrintRolWindow::byte_received(std::uint8_t b) {
    char c = static_cast<char>(b);
    QString str;
    str += c;
    ui->historyTextEdit->insertPlainText(str);
}

void PrintRolWindow::enter_on_combobox() {
    if (ui->portSelectBox->currentText().length() > 0) {
        connect_to_port();
    }
}
