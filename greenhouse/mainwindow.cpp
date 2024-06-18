#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init_net(); //初始化mqtt网络
    init_ui();  //初始化UI

    //初始化图表
    setupRealtimeDataDemo(ui->charts_1);
    setupRealtimeDataDemo(ui->charts_2);
    // 设置一个计时器，反复调用 MainWindow::realtimeDataSlot:
    //设置数据更新显示S
    connect(&dataTimer1, SIGNAL(timeout()), this, SLOT(realtimeDataSlot1()));
    connect(&dataTimer1, SIGNAL(timeout()), this, SLOT(realtimeDataSlot2()));
    dataTimer1.start(1000);
    //设置实时更新数据库
    connect(&dataTimer2, SIGNAL(timeout()), this, SLOT(save_envi_data()));
    dataTimer2.start(5000);
    //设置实时UI阈值显示
    connect(&dataTimer3, SIGNAL(timeout()), this, SLOT(ui_state()));
    dataTimer3.start(100);
    //连接mqtt服务器消息状态
    connect(m_client, &QMqttClient::stateChanged, this, &MainWindow::updateLogStateChange);
    //处理来自硬件数据
    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QLatin1String("T:")
                    + topic.name()
                    + QLatin1String("M:")
                    + message + "\n";
        ui->editLog->insertPlainText(content);
        QJsonParseError jsonError;
        QJsonDocument doucment = QJsonDocument::fromJson(message, &jsonError);  // 转化为 JSON 文档
        if(topic.name() == topic_hard){

            // 判断解析是否发生错误
            if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {
                if (doucment.isObject()) { // JSON 文档为对象
                    rx_flag = 1;
                    QJsonObject object = doucment.object();  // 转化为对象
                    if (object.contains("temp1")) {  // 包含指定的 key
                        QJsonValue value = object.value("temp1");  // 获取指定 key 对应的 value
                        if (value.isDouble()) {  // 判断 value 是否为数字
                            temp[0] = value.toInt();  // 将 value 转化为数字
                        }
                    }
                    if (object.contains("humi1")) {  // 包含指定的 key
                        QJsonValue value = object.value("humi1");  // 获取指定 key 对应的 value
                        if (value.isDouble()) {  // 判断 value 是否为数字
                            humi[0] = value.toInt();  // 将 value 转化为数字
                        }
                    }
                    if (object.contains("lux1")) {  // 包含指定的 key
                        QJsonValue value = object.value("lux1");  // 获取指定 key 对应的 value
                        if (value.isDouble()) {  // 判断 value 是否为数字
                            lux[0] = value.toInt();  // 将 value 转化为数字
                        }
                    }
                    if (object.contains("co21")) {  // 包含指定的 key
                        QJsonValue value = object.value("co21");  // 获取指定 key 对应的 value
                        if (value.isDouble()) {  // 判断 value 是否为数字
                            co2[0] = value.toInt();  // 将 value 转化为数字
                        }
                    }
                    if (object.contains("temp2")) {  // 包含指定的 key
                        QJsonValue value = object.value("temp2");  // 获取指定 key 对应的 value
                        if (value.isDouble()) {  // 判断 value 是否为数字
                            temp[1] = value.toInt();  // 将 value 转化为数字
                        }
                    }
                    if (object.contains("humi2")) {  // 包含指定的 key
                        QJsonValue value = object.value("humi2");  // 获取指定 key 对应的 value
                        if (value.isDouble()) {  // 判断 value 是否为数字
                            humi[1] = value.toInt();  // 将 value 转化为数字
                        }
                    }
                    if (object.contains("lux2")) {  // 包含指定的 key
                        QJsonValue value = object.value("lux2");  // 获取指定 key 对应的 value
                        if (value.isDouble()) {  // 判断 value 是否为数字
                            lux[1] = value.toInt();  // 将 value 转化为数字
                        }
                    }
                    if (object.contains("co22")) {  // 包含指定的 key
                        QJsonValue value = object.value("co22");  // 获取指定 key 对应的 value
                        if (value.isDouble()) {  // 判断 value 是否为数字
                            co2[1] = value.toInt();  // 将 value 转化为数字
                        }
                    }
                }
            }
        }
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::init_net()
{
    //系统初始化，建立一个Client(MQTT)
    m_client = new QMqttClient(this);
    m_client->setHostname("heclouds.com");
    m_client->setPort(6002);
    m_client->setUsername("489707");
    m_client->setPassword("exe");
    m_client->setClientId("935790048");
    m_client->setKeepAlive(120);

    this->setWindowTitle("大棚测控系统");
}
void MainWindow::init_ui()
{

}
void MainWindow::ui_state()
{
    if(mode == 1)
    {
        if((temp[0] > 60)||(humi[1] > 60))nc1 = 1;
        else nc1 = 0;
        if((humi[0] > 60)||(humi[1] > 60))nc2 = 1;
        else nc2 = 0;
        if((lux[0] > 60)||(lux[1] > 60))nc3 = 1;
        else nc3 = 0;
    }
    if(mode == 1)ui->mode_lb->setText("自动");
    else ui->mode_lb->setText("手动");
    if(nc1 == 1)ui->device1_lb->setText("打开");
    else ui->device1_lb->setText("关闭");
    if(nc2 == 1)ui->device2_lb->setText("打开");
    else ui->device2_lb->setText("关闭");
    if(nc3 == 1)ui->device3_lb->setText("打开");
    else ui->device3_lb->setText("关闭");
}
//初始化环境数据
void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
    if(customPlot == ui->charts_1)
    {
        customPlot->addGraph(); // blue line    温度
        customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
        customPlot->graph(0)->setName("温度");
        customPlot->addGraph(); // red line     湿度
        customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));
        customPlot->graph(1)->setName("湿度");
        customPlot->addGraph(); // green line   光强
        customPlot->graph(2)->setPen(QPen(QColor(102, 193, 140)));
        customPlot->graph(2)->setName("光强");
        customPlot->addGraph(); // yello line   CO2
        customPlot->graph(3)->setPen(QPen(QColor(100, 72, 61)));
        customPlot->graph(3)->setName("CO2");

        QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
        timeTicker->setTimeFormat("%h:%m:%s");
        customPlot->xAxis->setTicker(timeTicker);
        customPlot->axisRect()->setupFullAxesBox();//四边显示轴
        customPlot->yAxis->setRange(0, 600);
        customPlot->legend->setVisible(true);
    }else if(customPlot == ui->charts_2)
    {
        customPlot->addGraph(); // blue line    温度
        customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
        customPlot->graph(0)->setName("温度");
        customPlot->addGraph(); // red line     湿度
        customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));
        customPlot->graph(1)->setName("湿度");
        customPlot->addGraph(); // green line   光强
        customPlot->graph(2)->setPen(QPen(QColor(102, 193, 140)));
        customPlot->graph(2)->setName("光强");
        customPlot->addGraph(); // yello line   CO2
        customPlot->graph(3)->setPen(QPen(QColor(100, 72, 61)));
        customPlot->graph(3)->setName("CO2");

        QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
        timeTicker->setTimeFormat("%h:%m:%s");
        customPlot->xAxis->setTicker(timeTicker);
        customPlot->axisRect()->setupFullAxesBox();//四边显示轴
        customPlot->yAxis->setRange(0, 600);
        customPlot->legend->setVisible(true);
    }
}

void MainWindow::save_envi_data()
{
    QSqlQuery query;
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
    if(rx_flag == 1)
    {
        rx_flag = 0;
        QString str = QString("INSERT INTO environment VALUES('%1',%2,%3,%4)"
                              ).arg(current_date).arg(temp[0]).arg(humi[0]).arg(lux[0]);
        if(query.exec(str)==false){
            qDebug() << str;
            qDebug() << query.lastError();
        }
        else{
            qDebug() << "插入数据成功!";
            ui->editLog->insertPlainText("节点1数据保存成功!");
        }

        str = QString("INSERT INTO environment VALUES('%1',%2,%3,%4)"
                              ).arg(current_date).arg(temp[1]).arg(humi[1]).arg(lux[1]);

        if(query.exec(str)==false){
            qDebug() << str;
            qDebug() << query.lastError();
        }
        else{
            qDebug() << "插入数据成功!";
            ui->editLog->insertPlainText("节点2数据保存成功!");
        }
    }

}
//环境数据更新
void MainWindow::realtimeDataSlot1()
{
    static QTime timeStart = QTime::currentTime();
    // 计算两个新的数据点:
    double key = timeStart.msecsTo(QTime::currentTime())/1000.0; // 演示开始后经过的时间(以秒为单位)
    ui->charts_1->graph(0)->addData(key, temp[0]);
    ui->charts_1->graph(1)->addData(key, humi[0]);
    ui->charts_1->graph(2)->addData(key,lux[0]);
    ui->charts_1->graph(3)->addData(key,co2[0]);
    // 缩放值(垂直)轴以适应当前数据:
    //ui->customPlot->graph(0)->rescaleValueAxis();
    //ui->customPlot->graph(1)->rescaleValueAxis(true);

    // make key axis range scroll with the data (at a constant range size of 8):
    ui->charts_1->xAxis->setRange(key, 60, Qt::AlignRight);
    ui->charts_1->replot();
}


//环境数据更新
void MainWindow::realtimeDataSlot2()
{
    static QTime timeStart = QTime::currentTime();
    // 计算两个新的数据点:
    double key = timeStart.msecsTo(QTime::currentTime())/1000.0; // 演示开始后经过的时间(以秒为单位)
    ui->charts_2->graph(0)->addData(key, temp[1]);
    ui->charts_2->graph(1)->addData(key, humi[1]);
    ui->charts_2->graph(2)->addData(key,lux[1]);
    ui->charts_2->graph(3)->addData(key,co2[1]);
    // 缩放值(垂直)轴以适应当前数据:
    //ui->customPlot->graph(0)->rescaleValueAxis();
    //ui->customPlot->graph(1)->rescaleValueAxis(true);

    // make key axis range scroll with the data (at a constant range size of 8):
    ui->charts_2->xAxis->setRange(key, 60, Qt::AlignRight);
    ui->charts_2->replot();
}

//连接服务器状态更新函数
void MainWindow::updateLogStateChange()
{
    const QString content = QString::number(m_client->state());
    ui->State->setText("系统状态:"+content);
    if(m_client->state() == 2){
        auto subscription = m_client->subscribe(topic_hard);
        if (!subscription) {
            QMessageBox::critical(this, QLatin1String("Error"), "信息订阅失败，未连接服务器");
            return;
        }else QMessageBox::information(this, QLatin1String("OK"), "信息订阅成功，成功连接服务器");
    }
}

void MainWindow::on_buttonConnect_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected) {

        ui->buttonConnect->setText(tr("断开"));
        m_client->connectToHost();
    } else {
        ui->buttonConnect->setText(tr("连接"));
        m_client->disconnectFromHost();
    }

}


void MainWindow::on_mode_bt_clicked()
{
    if(mode == 0)
    {
        mode = 1;
        m_client->publish(topic_soft,"mode1");

    }else if(mode == 1)
    {
        mode = 0;
        m_client->publish(topic_soft,"mode0");
    }
}

void MainWindow::on_device1_bt_clicked()
{
    if(nc1 == 0)
    {
        nc1 = 1;
        m_client->publish(topic_soft,"nc11");
    }else if(nc1 == 1)
    {
        nc1 = 0;
        m_client->publish(topic_soft,"nc10");
    }

}

void MainWindow::on_device2_bt_clicked()
{
    if(nc2 == 0)
    {
        nc2 = 1;
        m_client->publish(topic_soft,"nc21");
    }else if(nc2 == 1)
    {
        nc2 = 0;
        m_client->publish(topic_soft,"nc20");
    }
}

void MainWindow::on_device3_bt_clicked()
{
    if(nc3 == 0)
    {
        nc3 = 1;
        m_client->publish(topic_soft,"nc31");
    }else if(nc3 == 1)
    {
        nc3 = 0;
        m_client->publish(topic_soft,"nc30");
    }
}

void MainWindow::on_device4_bt_clicked()
{
    if(nc4 == 0)
    {
        nc4 = 1;
        m_client->publish(topic_soft,"nc41");
    }else if(nc4 == 1)
    {
        nc4 = 0;
        m_client->publish(topic_soft,"nc40");
    }
}

void MainWindow::on_record_bt_clicked()
{
    datashow *dataw = new datashow;
    dataw->show();
    this->close();
}
