#include "printrol_window.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    PrintRolWindow w;
    w.show();
    return a.exec();
}
