#include "printrol_window.h"

#include <QApplication>
#include <ISerial/ISerial.h>

#if defined(WIN32)
    #include "WinSerial/WinSerial.h"
#endif

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    PrintRolWindow w;
    ISerial* serial = nullptr;
#if defined(WIN32)
    WinSerial wserial;
    serial = &wserial;
#endif
    w.set_serial(serial);
    w.init();
    w.show();
    return a.exec();
}
