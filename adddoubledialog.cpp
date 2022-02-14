#include "adddoubledialog.h"
#include "ui_adddoubledialog.h"

AddDoubleDialog::AddDoubleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDoubleDialog)
{
    ui->setupUi(this);
    outstring1 = outstring2 = "";
}

AddDoubleDialog::~AddDoubleDialog()
{
    delete ui;
}

void AddDoubleDialog::on_buttonBox_accepted()
{
    outstring1 = ui->lineEdit->text();
    outstring2 = ui->lineEdit_2->text();
}
