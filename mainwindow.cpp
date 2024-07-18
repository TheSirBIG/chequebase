#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "addsingledialog.h"
#include "adddoubledialog.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->actionCBox->setCheckState(Qt::Unchecked);
    ui->roundled->setState(QRoundLedLabel::StateError);
    ui->roundled->clear();
    on_reconnectButton_released();

    ui->prodCB->installEventFilter(this);
    ui->vendorCB->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    if(dbconnected) db.close();
    if(vendorList != nullptr) delete vendorList;
    if(storeList != nullptr) delete storeList;
    if(prodList != nullptr) delete prodList;
    if(prodListView != nullptr) delete prodListView;
    if(vendorListView != nullptr) delete vendorListView;
    delete ui;
}

bool MainWindow::eventFilter(QObject *inObject, QEvent *inEvent)
{
    static QString prodStr = "";
    static QString vendorStr = "";
//    std::cout << inObject->objectName().toStdString() << std::endl;
    QString *currStr = nullptr;
    QListView *currView = nullptr;
    QComboBox *currCB = nullptr;
    int currCount;

    QString objName = inObject->objectName();
    if((objName == "prodCB") || (objName == "prodListView"))
    {
        currStr = &prodStr;
        currView = prodListView;
        currCB = ui->prodCB;
        currCount = prodCount;
    }
    else if((objName == "vendorCB") || (objName == "vendorListView"))
    {
        currStr = &vendorStr;
        currView = vendorListView;
        currCB = ui->vendorCB;
        currCount = vendorCount;
    }
    else currStr = nullptr;

    if(currStr != nullptr)
        if(inEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(inEvent);
            bool backspace = (keyEvent->key() == Qt::Key_Backspace);
            bool del = (keyEvent->key() == Qt::Key_Delete);

            if((keyEvent->key() < 10000) || backspace || del) //exclude special keys, like CTRL
            {
                if(backspace)
                {
                    if(*currStr != "")
                    {
                        *currStr = currStr->remove(currStr->length()-1,1);
                    }
                }
                else if(del)
                {
                    *currStr = "";
                }
                else
                {
                    *currStr += keyEvent->text();
                }
                for(int i=0; i<currCount; i++)
                {
                    QString str = currCB->itemText(i);
                    bool b = str.contains(currStr,Qt::CaseInsensitive);
                    if(!b) currView->setRowHidden(i,true);
                            else currView->setRowHidden(i,false);
                }
                return true;
            }
            else return false;
        } else
        {
            return false;
        }
    else
    {
        return QObject::eventFilter(inObject, inEvent);
    }
}

void MainWindow::on_reconnectButton_released()
{
    if(dbconnected) db.close();
    dbconnected = DBConnect();
    if(dbconnected)
    {
        ui->roundled->setState(QRoundLedLabel::StateOk);
        GetStoreList();
        GetProdList();
        GetVendorList();
    }
    else
    {
        ui->roundled->setState(QRoundLedLabel::StateError);
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
               quantity double not null,\
               price double not null,\
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
//    if(vendorListView != nullptr) delete vendorListView;
    vendorList = nullptr;
//    vendorListView = nullptr;
    vendorCount = 0;
    ui->vendorCB->clear();
    vendorListView = new QListView(ui->vendorCB);
    vendorListView->setObjectName("vendorListView");
    ui->vendorCB->setView(vendorListView);
    query = QSqlQuery(db);
    query.exec("select * from vendor");
    if(query.isSelect())
    {
        if(query.size()>0) vendorList = new int[query.size()];
        while(query.next())
        {
            vendorList[i++] = query.value(0).toInt();
            ui->vendorCB->addItem(query.value(1).toString());
            vendorCount++;
        }
    }
    vendorListView->setFixedHeight(150);  //for 10 lines in combobox!!!
    vendorListView->installEventFilter(this);
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
//    if(prodListView != nullptr) delete prodListView;
    prodList = nullptr;
//    prodListView = nullptr;
    prodCount = 0;
    ui->prodCB->clear();
    prodListView = new QListView(ui->prodCB);
    prodListView->setObjectName("prodListView");
    ui->prodCB->setView(prodListView);
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
                str += " : ";
                str += query.value(2).toString();
            }
            prodList[i++] = query.value(0).toInt();
            ui->prodCB->addItem(str);
            prodCount++;
        }
    }
    prodListView->setFixedHeight(150);  //for 10 lines in combobox!!!
    prodListView->installEventFilter(this);
}

void MainWindow::SetViewVisible(QListView *view, int count)
{
    for(int i=0; i<count; i++) view->setRowHidden(i,false);
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
        GetVendorList();
        ui->vendorCB->setCurrentIndex(ui->vendorCB->count()-1);
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
        GetStoreList();
        ui->storeCB->setCurrentIndex(ui->storeCB->count()-1);
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
        GetProdList();
        ui->prodCB->setCurrentIndex(ui->prodCB->count()-1);
    }
    delete dialog;
}

void MainWindow::on_addButton_released()
{
    double quantity, price;
    bool ok;
    QString res;
    QDate date;

    res = ui->quantityEdit->text();
    for(int i=0; i<res.length(); i++)
        if(res[i] == ',') res[i] = '.';
    quantity = res.toDouble(&ok);
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
        price = res.toDouble(&ok);
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
                SetViewVisible(prodListView, prodCount);
                SetViewVisible(vendorListView, vendorCount);
            }
        }
    }
}
