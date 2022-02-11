#ifndef DIALOG1_H
#define DIALOG1_H

#include <QDialog>
#include <QMainWindow>

namespace Ui {
class Dialog1;
}

class Dialog1 : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog1(QWidget *parent = nullptr);
    ~Dialog1();
    void setparent(QMainWindow *www);

private slots:
    void on_pushButton_released();

private:
    Ui::Dialog1 *ui;
    QMainWindow *main;
};

#endif // DIALOG1_H
