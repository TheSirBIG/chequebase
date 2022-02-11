#ifndef ADDDOUBLEDIALOG_H
#define ADDDOUBLEDIALOG_H

#include <QDialog>

namespace Ui {
class AddDoubleDialog;
}

class AddDoubleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDoubleDialog(QWidget *parent = nullptr);
    ~AddDoubleDialog();

private:
    Ui::AddDoubleDialog *ui;

public:
    QString outstring1, outstring2;
};

#endif // ADDDOUBLEDIALOG_H
