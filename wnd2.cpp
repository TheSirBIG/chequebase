#include "wnd2.h"
#include "ui_wnd2.h"

Wnd2::Wnd2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Wnd2)
{
    ui->setupUi(this);
}

Wnd2::~Wnd2()
{
    delete ui;
}

void Wnd2::show1(QMainWindow *wnd)
{
    main = wnd;
    this->show();
}

void Wnd2::on_pushButton_released()
{
    this->close();
    main->show();
}
