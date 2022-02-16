#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_reconnectButton_released();

    void on_pushButton_released();

    void on_addVendorButton_released();

    void on_addStoreButton_released();

    void on_addProdButton_released();

private:
    bool DBConnect();
    void GetVendorList();
    void GetStoreList();
    void GetProdList();

private:
    Ui::MainWindow *ui;
    bool dbconnected;
    QSqlError lastError;
    QSqlDatabase db;
    int *vendorList = nullptr;
    int *storeList = nullptr;
    int *prodList = nullptr;

};
#endif // MAINWINDOW_H
