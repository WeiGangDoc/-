#ifndef REGISTERED_H
#define REGISTERED_H

#include <QMainWindow>

namespace Ui {
class registered;
}

class registered : public QMainWindow
{
    Q_OBJECT

public:
    explicit registered(QWidget *parent = nullptr);
    ~registered();

private slots:
    void on_back_bt_clicked();

    void on_register_bt_clicked();

private:
    Ui::registered *ui;
};

#endif // REGISTERED_H
