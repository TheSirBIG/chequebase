#include "addsingledialog.h"
#include "ui_addsingledialog.h"

AddSingleDialog::AddSingleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSingleDialog)
{
    ui->setupUi(this);
    outstring = "";
    ui->lineEdit->setFocus();
}

AddSingleDialog::~AddSingleDialog()
{
    delete ui;
}

void AddSingleDialog::on_buttonBox_accepted()
{
    outstring = ui->lineEdit->text();
}

void AddSingleDialog::SetLabelText(QString labeltext)
{
    ui->label->setText(labeltext);
}
