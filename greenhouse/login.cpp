#include "login.h"
#include "ui_login.h"

QString login_identify;

login::login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    this->setWindowTitle("大棚测控系统");
    init_logindb();
    creat_loginTable();
}

login::~login()
{
    delete ui;
}

//创建数据库
void login::init_logindb()
{
    //添加数据库驱动库
    logindb = QSqlDatabase::addDatabase("QSQLITE");
    //设置数据库名字(文件名)
    logindb.setDatabaseName("message.db");
    //打开数据库
    if(logindb.open()==true){
        qDebug() << "创建/打开数据库成功!";
    }
    else{
        qDebug() << "创建/打开数据库失败!";
    }
}
//创建数据表
void login::creat_loginTable()
{
   QSqlQuery query;
   QString str = QString("CREATE TABLE environment ("
                         "time varchar NOT NULL,"
                         "temp varchar NOT NULL,"
                         "humi varchar NOT NULL,"
                         "lux varchar NOT NULL)");
   if(query.exec(str) == false){
       qDebug() << str;
       qDebug() << "Error: Fail to create table."<< query.lastError();
   }
   else{
       qDebug() << "创建环境数据表成功!";
   }
   str = QString("CREATE TABLE setval ("
                 "time varchar NOT NULL,"
                 "tempv varchar NOT NULL,"
                 "humiv varchar NOT NULL,"
                 "luxv varchar NOT NULL)");
   if(query.exec(str) == false){
       qDebug() << str;
       qDebug() << "Error: Fail to create table."<< query.lastError();
   }
   else{
       qDebug() << "创建环境数据阈值表成功!";
   }
   str = QString("CREATE TABLE people ("
                            "name varchar NOT NULL,"
                            "pwd  varchar NOT NULL,"
                            "phone varchar NOT NULL,"
                            "gender varchar NOT NULL,"
                            "identify varchar NOT NULL)");

      if(query.exec(str) == false){
          qDebug() << str;
          qDebug() << "Error: Fail to create table."<< query.lastError();
      }
      else{
          qDebug() << "创建个人信息数据表成功!";
      }
}

void login::on_login_bt_clicked()
{
    QString phonestr = ui->phone_le->text();
    QString pwdstr = ui->pwd_le->text();
    QString identifystr = ui->identify_le->text();
    if((phonestr == "")||(pwdstr =="")||(identifystr==""))
        QMessageBox::warning(this,"","请输入完整信息");

    QSqlQuery query;
    QString str = QString(QString("select * from people where phone='%1' and pwd='%2'")).arg(phonestr).arg(pwdstr);
    query.exec(str);
    if(query.first())
      {
       QMessageBox::information(this,"登录情况","登录成功");
       MainWindow *w = new MainWindow;
       w->show();
       this->close();
      }
    else
        QMessageBox::critical(this,"登录情况","密码不正确！");

}

void login::on_register_bt_clicked()
{
    registered *registerw = new registered;
    registerw->show();
    this->close();
}

void login::on_quit_bt_clicked()
{
    this->close();
}
