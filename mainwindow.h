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

    bool eventFilter( QObject * inObject, QEvent * inEvent ) override;

private slots:
    void on_reconnectButton_released();

    void on_addVendorButton_released();

    void on_addStoreButton_released();

    void on_addProdButton_released();

    void on_addButton_released();

private:
    bool DBConnect();
    void GetVendorList();
    void GetStoreList();
    void GetProdList();
    void SetViewVisible(QListView *view, int count, QString *filter);
    void ApplyFilter(QComboBox *box, QListView *view, int count, QString *filter);

private:
    Ui::MainWindow *ui;
    bool dbconnected = false;
    QSqlError lastError;
    QSqlDatabase db;
    int *vendorIDList = nullptr;
    int *storeIDList = nullptr;
    int *prodIDList = nullptr;

    QListView *prodListView = nullptr;
    QListView *vendorListView = nullptr;

    int vendorCount = 0;
    int prodCount = 0;

    QString prodStr = "";
    QString vendorStr = "";

    int CBScale = 150;
};
#endif // MAINWINDOW_H
