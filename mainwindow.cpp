#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "addsingledialog.h"
#include "adddoubledialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->actionCBox->setCheckState(Qt::Unchecked);
    on_reconnectButton_released();
}

MainWindow::~MainWindow()
{
    if(dbconnected) db.close();
    if(vendorList != nullptr) delete vendorList;
    if(storeList != nullptr) delete storeList;
    if(prodList != nullptr) delete prodList;
    delete ui;
}

void MainWindow::on_reconnectButton_released()
{
    if(dbconnected) db.close();
    dbconnected = DBConnect();
    if(dbconnected)
    {
        GetStoreList();
        GetProdList();
        GetVendorList();
    }
}

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
    query.exec("use cheque");
    query.exec("create table if not exists vendor(\
               keyid int AUTO_INCREMENT PRIMARY KEY,\
               name varchar(255) not null)");
    query.exec("create table if not exists store(\
               keyid int AUTO_INCREMENT PRIMARY KEY,\
               name varchar(255) not null)");
    query.exec("create table if not exists prod(\
               keyid int AUTO_INCREMENT PRIMARY KEY,\
               name varchar(255) not null,\
               subname varchar(255) default null)");
    query.exec("create table if not exists chequelist(\
               keyid int AUTO_INCREMENT PRIMARY KEY,\
               dateofbuy date not null,\
               storeid int,\
               vendorid int,\
               prodid int,\
               quantity float not null,\
               price float not null,\
               action bool not null default false,\
               foreign key (storeid) references store(keyid),\
               foreign key (vendorid) references vendor(keyid),\
               foreign key (prodid) references prod(keyid),\
               index (prodid))");
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

void MainWindow::GetStoreList()
{
    QSqlQuery query;
    int i = 0;

    if(storeList != nullptr) delete storeList;
    storeList = nullptr;
    ui->storeCB->clear();
    query = QSqlQuery(db);
    query.exec("select * from store");
    if(query.isSelect())
    {
        if(query.size()>0) storeList = new int[query.size()];
        while(query.next())
        {
            storeList[i++] = query.value(0).toInt();
            ui->storeCB->addItem(query.value(1).toString());
        }
    }
}

void MainWindow::GetProdList()
{
    QSqlQuery query;
    QString str;
    int i = 0;

    if(prodList != nullptr) delete prodList;
    prodList = nullptr;
    ui->prodCB->clear();
    query = QSqlQuery(db);
    query.exec("select * from prod");
    if(query.isSelect())
    {
        if(query.size()>0) prodList = new int[query.size()];
        while(query.next())
        {
            str = query.value(1).toString();
            if(query.value(2) != "")
            {
                str += ", ";
                str += query.value(2).toString();
            }
            prodList[i++] = query.value(0).toInt();
            ui->prodCB->addItem(str);
        }
    }
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
    GetVendorList();
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
    GetStoreList();
}

void MainWindow::on_addProdButton_released()
{
    AddDoubleDialog *dialog = new AddDoubleDialog();

    if(dialog->exec() == QDialog::Accepted)
    {
        QSqlQuery query = QSqlQuery(db);
        if(dialog->outstring2 != "")
        {
            query.prepare("insert into prod(name,subname) values(:name,:subname)");
            query.bindValue(":name", dialog->outstring1);
            query.bindValue(":subname", dialog->outstring2);
        }
        else
        {
            query.prepare("insert into prod(name) values(:name)");
            query.bindValue(":name", dialog->outstring1);
        }
        if(!query.exec())
        {
            QMessageBox::critical(this,"Error","Query error:\n"+query.lastError().text());
        }
    }
    delete dialog;
    GetProdList();
}

void MainWindow::on_addButton_released()
{
    float quantity, price;
    bool ok;
    QString res;
    QDate date;

    res = ui->quantityEdit->text();
    for(int i=0; i<res.length(); i++)
        if(res[i] == ',') res[i] = '.';
    quantity = res.toFloat(&ok);
    if(!ok)
    {
        QMessageBox::critical(this,"Error","Error in quantity edit\n");
        ui->quantityEdit->clear();
    }
    else
    {
        res = ui->priceEdit->text();
        for(int i=0; i<res.length(); i++)
            if(res[i] == ',') res[i] = '.';
        price = res.toFloat(&ok);
        if(!ok)
        {
            QMessageBox::critical(this,"Error","Error in price edit\n");
            ui->priceEdit->clear();
        }
        else
        {
            date = ui->dateEdit->date();
            QSqlQuery query = QSqlQuery(db);
            query.prepare("INSERT INTO chequelist(dateofbuy,storeid,vendorid,prodid,quantity,price,action) VALUES (:dateofbuy,:storeid,:vendorid,:prodid,:quantity,:price,:action)");
            res = QString::number(date.year())+"-"+QString::number(date.month())+"-"+QString::number(date.day());
            query.bindValue(":dateofbuy", res);
            query.bindValue(":storeid", storeList[ui->storeCB->currentIndex()]);
            query.bindValue(":vendorid", vendorList[ui->vendorCB->currentIndex()]);
            query.bindValue(":prodid", prodList[ui->prodCB->currentIndex()]);
            query.bindValue(":quantity", quantity);
            query.bindValue(":price", price);
            if(ui->actionCBox->checkState() == Qt::Checked)
            {
                query.bindValue(":action", 1);
            }
            else
            {
                query.bindValue(":action", 0);
            }
            if(!query.exec())
            {
                QMessageBox::critical(this,"Error","Query error:\n"+query.lastError().text());
            }
            else
            {
                ui->priceEdit->clear();
                ui->quantityEdit->clear();
                ui->actionCBox->setCheckState(Qt::Unchecked);
            }
        }
    }
}
