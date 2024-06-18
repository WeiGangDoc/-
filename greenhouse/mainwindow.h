#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtMqtt/qmqttclient.h"
#include <qstring.h>
#include "QTimer"
#include "qcustomplot.h"
#include"QtSql/QSqlQuery"
#include "QtSql/QSqlQueryModel"

extern QString login_identify;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void init_net();
    void init_ui();
    void ui_state();
    void updateLogStateChange();//连接服务器状态更新函数
    void setupRealtimeDataDemo(QCustomPlot *customPlot);//初始化图表函数
    void save_envi_data();
    void realtimeDataSlot1();
    void realtimeDataSlot2();
    void on_buttonConnect_clicked();
    void on_mode_bt_clicked();
    void on_device1_bt_clicked();
    void on_device2_bt_clicked();
    void on_device3_bt_clicked();
    void on_device4_bt_clicked();
    void on_record_bt_clicked();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer1,dataTimer2,dataTimer3;
    QMqttClient *m_client;
    qint16 temp[2],humi[2],lux[2],co2[2];
    //topic定义
    QString topic_hard = "hard";
    QString topic_soft = "soft";
    qint8 rx_flag = 0;
    qint8 mode = 0,nc1 = 0,nc2 = 0,nc3 = 0,nc4 = 0;
};
#endif // MAINWINDOW_H
