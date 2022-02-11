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
