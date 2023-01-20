#ifndef PRINTROLWINDOW_H
#define PRINTROLWINDOW_H

#include <QMainWindow>

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

private:
    Ui::PrintRolWindow* ui;
};
#endif  // PRINTROLWINDOW_H
