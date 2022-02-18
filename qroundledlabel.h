//Code by Gojir4
//https://forum.qt.io/topic/101648/how-to-create-simply-virtual-led-indicator
#ifndef QROUNDLEDLABEL_H
#define QROUNDLEDLABEL_H

#include <QLabel>

class QRoundLedLabel : public QLabel
{
    Q_OBJECT
public:
    explicit QRoundLedLabel(QWidget *parent = 0);

    enum State{
        StateOk,
        StateOkBlue,
        StateWarning,
        StateError
    };


signals:

public slots:
    void setState(State state);
    void setState(bool state);
};

#endif // QROUNDLEDLABEL_H
