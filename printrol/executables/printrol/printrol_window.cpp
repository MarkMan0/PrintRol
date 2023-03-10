#include "printrol_window.h"
#include "./ui_printrol_window.h"

#include <vector>
#include <string>
#include <QScrollBar>
#include <QCursor>

PrintRolWindow::PrintRolWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::PrintRolWindow) {
    ui->setupUi(this);

    connect(ui->portRefreshButton, &QPushButton::clicked, this, &PrintRolWindow::refresh_ports);
    connect(ui->portConnectButton, &QPushButton::clicked, this, &PrintRolWindow::connect_to_port);
    connect(ui->portDisconnectButton, &QPushButton::clicked, this, &PrintRolWindow::disconnect_port);
    connect(ui->inputLineEdit, &QLineEdit::returnPressed, this, &PrintRolWindow::user_txt_input);
    connect(&comm_thrd_, &CommThread::line_received, this, &PrintRolWindow::line_received);
    connect(ui->portSelectBox->lineEdit(), &QLineEdit::returnPressed, this, &PrintRolWindow::enter_on_combobox);
    connect(ui->portBaudComboBox->lineEdit(), &QLineEdit::returnPressed, this, &PrintRolWindow::enter_on_combobox);
    connect(&comm_thrd_, &CommThread::printer_status_changed, this, &PrintRolWindow::printer_status_change);

    const auto baud_list = { 9600, 14400, 19200, 38400, 57600, 115200, 128000, 256000, 1000000 };

    for (auto baud : baud_list) {
        ui->portBaudComboBox->addItem(QString::number(baud));
    }

    ui->portBaudComboBox->setCurrentIndex(5);

    filter_.add_filter(LineFilter::temperature_regex());
    filter_.add_filter(LineFilter::position_regex());
}

void PrintRolWindow::init() {
    comm_thrd_.set_serial(serial_);

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

    int baud = ui->portBaudComboBox->currentText().toInt();
    if (baud == 0) {
        baud = 115200;
    }

    serial_->open(current_port.toStdWString(), baud);

    if (comm_thrd_.isRunning()) {
        comm_thrd_.abort();
        while (comm_thrd_.isRunning()) {
        }
    }
    comm_thrd_.start(QThread::NormalPriority);

    update_port_label();
}

void PrintRolWindow::disconnect_port() {
    serial_->close();
    comm_thrd_.abort();
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

void PrintRolWindow::line_received(std::string str) {
    if (not filter_.check(str)) {
        return;
    }

    auto& bar = *ui->historyTextEdit->verticalScrollBar();
    bool scroll = bar.maximum() == bar.sliderPosition();
    auto old_cursor = ui->historyTextEdit->textCursor();
    ui->historyTextEdit->moveCursor(QTextCursor::End);
    ui->historyTextEdit->insertPlainText(QString::fromStdString(str));

    ui->historyTextEdit->setTextCursor(old_cursor);

    if (scroll) {
        bar.setSliderPosition(bar.maximum());
    }
}

void PrintRolWindow::enter_on_combobox() {
    if (ui->portSelectBox->currentText().length() > 0) {
        connect_to_port();
    }
}

void PrintRolWindow::printer_status_change() {
    const auto& printer = comm_thrd_.get_printer();
    {
        const auto pos = printer.get_position();
        if (pos.size() >= 4) {
            ui->xPosLabel->setText(QString("X:") + QString::number(pos[0]));
            ui->yPosLabel->setText(QString("Y:") + QString::number(pos[1]));
            ui->zPosLabel->setText(QString("Z:") + QString::number(pos[2]));
            ui->ePosLabel->setText(QString("E:") + QString::number(pos[3]));
        }
    }
    {
        const auto h0_temp = printer.get_hotend_temp();
        if (h0_temp.has_value()) {
            ui->eTempAct->setText(QString::number(h0_temp->actual));
            ui->eTempSet->setText(QString::number(h0_temp->set));
            ui->eTempPower->setText(QString::number(h0_temp->power));
        }
    }
    {
        const auto bed_temp = printer.get_bed_temp();
        if (bed_temp.has_value()) {
            ui->bedTempAct->setText(QString::number(bed_temp->actual));
            ui->bedTempSet->setText(QString::number(bed_temp->set));
            ui->bedTempPower->setText(QString::number(bed_temp->power));
        }
    }
}
