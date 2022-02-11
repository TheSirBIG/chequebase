#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <iostream>
#include "addsingledialog.h"
#include "adddoubledialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dbconnected = DBConnect();

    window_2 = new Wnd2();
    dialog1 = new Dialog1(this);
//    dialog1 = new Dialog1();
}

MainWindow::~MainWindow()
{
    if(dbconnected) db.close();
    delete window_2;
    delete dialog1;
    if(vendorList != nullptr) delete vendorList;
    if(storeList != nullptr) delete storeList;
    if(prodList != nullptr) delete prodList;
    delete ui;
}

/*
void MainWindow::on_pushButton_released()
{
    this->close();
    window_2->show1(this);
}

void MainWindow::on_pushButton_2_released()
{
    //dialog1->show();
    dialog1->exec();
}
*/
bool MainWindow::DBConnect()
{
    QSqlQuery query;

    //connecting
    db = QSqlDatabase::addDatabase("QMYSQL","conn_name1");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("12345");
    if(!db.open())
    {
        QMessageBox::critical(this,"Error","Database error:\n"+db.lastError().text());
        lastError = db.lastError();
        return(false);
    }
    //creating database if not exists
    query = QSqlQuery(db);
    query.exec("CREATE DATABASE if not exists cheque DEFAULT CHARACTER SET cp1251 COLLATE cp1251_general_ci");
//    query.exec("CREATE DATABASE if not exists cheque");
    query.exec("use cheque");
    query.exec("create table if not exists vendor(\
               keyid int AUTO_INCREMENT PRIMARY KEY,\
               name varchar(255) not null)");
    query.exec("create table if not exists store(\
               keyid int AUTO_INCREMENT PRIMARY KEY,\
               name varchar(255))");
    query.exec("create table if not exists prodtype(\
               keyid int AUTO_INCREMENT PRIMARY KEY,\
               type varchar(255) not null,\
               subtype varchar(255))");
    query.exec("create table if not exists chequelist(\
               keyid int AUTO_INCREMENT PRIMARY KEY,\
               dateofbuy date not null default CURRENT_DATE,\
               storeid int,\
               vendorid int,\
               prodtypeid int not null,\
               prodsubtypeid int,\
               quantity float not null,\
               price float not null,\
               action bool not null default false,\
               foreign key (storeid) references store(keyid),\
               foreign key (vendorid) references vendor(keyid),\
               foreign key (prodtypeid) references prodtype(keyid),\
               foreign key (prodsubtypeid) references prodsubtype(keyid),\
               index (prodtypeid))");

//    query.exec("insert into vendor(name) values('магнит')");
//    if(!query.exec("insert into vendor(name) values('магнит')"))
//        std::cout << std::string(query.lastError().text().toUtf8()) << std::endl;
            return(true);
}

void MainWindow::GetVendorList()
{
    QSqlQuery query;
    int i = 0;

    if(vendorList != nullptr) delete vendorList;
    vendorList = nullptr;
    ui->vendorCB->clear();
    query = QSqlQuery(db);
    query.exec("select * from vendor");
    if(query.isSelect())
    {
        if(query.size()>0) vendorList = new int[query.size()];
        while(query.next())
        {
            vendorList[i++] = query.value(0).toInt();
            ui->vendorCB->addItem(query.value(1).toString());
        }
    }
}

void MainWindow::on_reconnectButton_released()
{
    if(!dbconnected) dbconnected = DBConnect();
}

void MainWindow::on_pushButton_released()
{
    GetVendorList();
}

void MainWindow::on_addVendorButton_released()
{
    AddSingleDialog *dialog = new AddSingleDialog();

    dialog->SetLabelText("Наименование производителя:");
    if(dialog->exec() == QDialog::Accepted)
    {
        QSqlQuery query = QSqlQuery(db);
        query.prepare("insert into vendor(name) values(:name)");
        query.bindValue(":name", dialog->outstring);
        if(!query.exec())
        {
            QMessageBox::critical(this,"Error","Query error:\n"+query.lastError().text());
        }
    }
    delete dialog;
}

void MainWindow::on_addStoreButton_released()
{
    AddSingleDialog *dialog = new AddSingleDialog();

    dialog->SetLabelText("Наименование магазина, адрес:");
    if(dialog->exec() == QDialog::Accepted)
    {
        QSqlQuery query = QSqlQuery(db);
        query.prepare("insert into store(name) values(:name)");
        query.bindValue(":name", dialog->outstring);
        if(!query.exec())
        {
            QMessageBox::critical(this,"Error","Query error:\n"+query.lastError().text());
        }
    }
    delete dialog;
}

void MainWindow::on_addProdButton_released()
{
    AddDoubleDialog *dialog = new AddDoubleDialog();

    if(dialog->exec() == QDialog::Accepted)
    {
        QSqlQuery query = QSqlQuery(db);
        if(dialog->outstring2 != "")
        {
            query.prepare("insert into prodtype(type,subtype) values(:type,:subtype)");
            query.bindValue(":type", dialog->outstring1);
            query.bindValue(":subtype", dialog->outstring2);
        }
        else
        {

        }
        if(!query.exec())
        {
            QMessageBox::critical(this,"Error","Query error:\n"+query.lastError().text());
        }
    }
    delete dialog;
}
