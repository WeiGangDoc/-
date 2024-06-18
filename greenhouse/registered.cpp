#include "registered.h"
#include "ui_registered.h"
#include "login.h"
#include "QDebug"
registered::registered(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::registered)
{
    ui->setupUi(this);
    this->setWindowTitle("大棚测控系统");
}

registered::~registered()
{
    delete ui;
}

void registered::on_back_bt_clicked()
{
    login *w = new login;
    w->show();
    this->close();
}

void registered::on_register_bt_clicked()
{
    QString namestr = ui->name_le->text();
    QString pwdstr = ui->pwd_le->text();
    QString phonestr = ui->phone_le->text();
    QString genderstr = ui->gender_le->text();
    QString identifystr = ui->identify_le->text();
    QSqlQuery query;
    if((namestr == "")||(pwdstr == "")||(phonestr == "")||(genderstr == "")||(identifystr == ""))
    {
        QMessageBox::warning(NULL,"warn","请输入完整信息");
    }else
    {
        QString str = QString("INSERT INTO people VALUES('%1','%2','%3','%4','%5')"
                              ).arg(namestr).arg(pwdstr).arg(phonestr).arg(genderstr).arg(identifystr);
        if(query.exec(str)==false){
            qDebug() << str;
            qDebug() << query.lastError();
        }
        else{
            qDebug() << "录入信息成功!";
            QMessageBox::warning(NULL,"good","用户信息注册成功");
        }
    }

}
