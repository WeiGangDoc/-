#ifndef DATASHOW_H
#define DATASHOW_H

#include <QMainWindow>
#include "login.h"
namespace Ui {
class datashow;
}

class datashow : public QMainWindow
{
    Q_OBJECT

public:
    explicit datashow(QWidget *parent = nullptr);
    ~datashow();

private slots:
    void queryTable();  //查询
    void on_back_bt_clicked();

private:
    Ui::datashow *ui;
    QSqlQueryModel model;//保存结果集
};

#endif // DATASHOW_H
