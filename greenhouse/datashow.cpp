#include "datashow.h"
#include "ui_datashow.h"

datashow::datashow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::datashow)
{
    ui->setupUi(this);
    //设置数据列表自适应宽度
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setStretchLastSection(false);
    queryTable();

}

datashow::~datashow()
{
    delete ui;
}

void datashow::queryTable()
{
    QString str = QString("SELECT * FROM environment");
    model.setQuery(str);
    ui->tableView->setModel(&model);
}
void datashow::on_back_bt_clicked()
{
    MainWindow *w = new MainWindow;
    w->show();
    this->close();
}
