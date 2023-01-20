#include "printrol_window.h"
#include "./ui_printrol_window.h"

#include <vector>
#include <string>

PrintRolWindow::PrintRolWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::PrintRolWindow) {
    ui->setupUi(this);

    connect(ui->portRefreshButton, &QPushButton::clicked, this, &PrintRolWindow::refresh_ports);
    connect(ui->portConnectButton, &QPushButton::clicked, this, &PrintRolWindow::connect_to_port);
    connect(ui->portDisconnectButton, &QPushButton::clicked, this, &PrintRolWindow::disconnect_port);

    refresh_ports();
    update_port_label();
}

PrintRolWindow::~PrintRolWindow() {
    delete ui;
}

void PrintRolWindow::refresh_ports() {
    // TODO: get list of actual ports
    std::vector<QString> ports;
    ports.push_back("COM0");
    ports.push_back("COM1");
    ports.push_back("COM9");
    ports.push_back("COM10");
    ports.push_back("COM11");

    ui->portSelectBox->clear();
    for (const auto& port : ports) {
        ui->portSelectBox->addItem(port);
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
