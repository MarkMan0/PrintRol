#include "printrol_window.h"
#include "./ui_printrol_window.h"

PrintRolWindow::PrintRolWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::PrintRolWindow) {
    ui->setupUi(this);
}

PrintRolWindow::~PrintRolWindow() {
    delete ui;
}
