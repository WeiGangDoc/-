#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include"QtSql/QSqlQuery"
#include "QtSql/QSqlQueryModel"
#include "QtSql/QSqlError"
#include "QDebug"
#include "QMessageBox"
#include "registered.h"
#include "mainwindow.h"
#include "datashow.h"
namespace Ui {
class login;
}

class login : public QMainWindow
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private slots:
    void init_logindb();//初始化登录信息数据库
    void creat_loginTable();//初始化登录信息数据表
    void on_login_bt_clicked();
    void on_register_bt_clicked();
    void on_quit_bt_clicked();

private:
    Ui::login *ui;
    QSqlDatabase logindb;//建立Qt和数据库链接
};

#endif // LOGIN_H
