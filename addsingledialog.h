#ifndef ADDSINGLEDIALOG_H
#define ADDSINGLEDIALOG_H

#include <QDialog>

namespace Ui {
class AddSingleDialog;
}

class AddSingleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddSingleDialog(QWidget *parent = nullptr);
    ~AddSingleDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AddSingleDialog *ui;

public:
    QString outstring;
//    QString labeltext;
    void SetLabelText(QString labeltext);
};

#endif // ADDSINGLEDIALOG_H
