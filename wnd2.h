#ifndef WND2_H
#define WND2_H

#include <QMainWindow>

namespace Ui {
class Wnd2;
}

class Wnd2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit Wnd2(QWidget *parent = nullptr);
    ~Wnd2();
    void show1(QMainWindow *wnd);

private slots:
    void on_pushButton_released();

private:
    Ui::Wnd2 *ui;
    QMainWindow *main;
};

#endif // WND2_H
