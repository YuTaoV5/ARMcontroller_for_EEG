#include "mainwindow.h"
#include <QtSql>
#include "ui_mainwindow.h"
#include <QDebug>
#include "hidapi.h"
#include <QPixmap>
#include <QtMath>
#define MAX(a,b,c) (a>b?(a>c?a:c):(b>c?b:c))
#define MIN(a,b,c) (a<b)?((a<c)?a:c):(b<c?b:c)
#define _ilimit( a, b, c )  ((a) < (b) ? (b) : ((a) > (c) ? (c) : (a)))   //限幅 a原始 b小 c大
#define l1 12
#define l2 12

_POINT_ P_A={-10,0,11.5,0};
_POINT_ P_B={-10,10,11.5,0};
_POINT_ P_C={-10,20,11.5,0};
   float h_A;//A台高度
   float h_B;//B高度
   float h_C;//C高度
_POINT_ target;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    P_now={
        .x = -3.5,
        .y = 0,
        .z = 8,
        .q = 0
    };
     P_org={
        .x = -3.5,
        .y = 0,
        .z = 8,
        .q = 0
    };
     P_CatchUp={//移动到物体上方
        .x = -12,
        .y = 0,
        .z = 4
    };
     P_CatchDown={//移动到物体上
        .x = -12,
        .y = 0,
        .z = 1.5
    };
     P_CatchSky={//移动到物体上
        .x = -12,
        .y = 0,
        .z = 8
    };
    ui->setupUi(this);
    signal_ele=0;
    signal_qiB=0;
    signal_deliver=0;
    ReturnFlag=0;
    Mytime = 10;
    zitai_count = 1;
    path = "D:/SSVEP_Paradigm/command.txt";
    now_txt = "0";
    last_txt = now_txt;
    // 点击选取文件按钮，弹出文件对话框
    connect(ui->pushButton_open, &QPushButton::clicked, [=]{
        path = QFileDialog::getOpenFileName(this, "打开文件", "D:/SSVEP_Paradigm");
        // 将路径放入到lineEdit中
        ui->lineEdit_file->setText(path);

        // 读取内容 放入到textEdt中
        QFile file(path);  // 参数就是选取文件的路径
        // 设置打开方式
        file.open(QIODevice::ReadOnly);

        QByteArray array = file.readAll();
        // 将读取到的数据 放入到textEdt中
        ui->textEdit_file->setText(array);
        });
    QTimer *timer_EEG = new QTimer(this);
    connect(timer_EEG, SIGNAL(timeout()), this, SLOT(Timer_func()));
    timer_EEG->start(100);
    //创建表格
       createConnection();
        mymodel = new QSqlTableModel(this);
        mymodel->setTable("HugeSmart_ARM");
        mymodel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        mymodel->select();
       ui->tableView->setModel(mymodel);
    //创建串口对象
    serial_port_t = new QSerialPort();//机械臂串口
    //绑定读取串口信息的信号和槽
    connect(serial_port_t, &QSerialPort::readyRead, this, &MainWindow::read_date);
    //TCP
    this->socket = new QTcpSocket(this);
    //获取允许使用的串口信息
    QList<QSerialPortInfo> port = QSerialPortInfo::availablePorts();
    if(port.isEmpty())
        ui->com->addItem("无效");
    else
    {
        foreach(const QSerialPortInfo &port_info, port)
        {
            ui->com->addItem(port_info.portName());
        }
    }
    m_bIsOpenPort = false;

    //进度条
    ui->progressBar->setValue(0);

    //步进数据
    Arm_x=0;
    Arm_y=0;
    Arm_z=0;
    Arm_u=0;
    EEG_q = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::send_zitai_data()
{    
    if(ReturnFlag==1 && controlflag==4)
       serial_port_t->write(ba); //x轴归位
}
float judge(float a,float b)
{
    if(a==b)return 0.0f;
    if(a>b)return -1.0f;
    if(a<b)return 1.0f;
}
void MainWindow::show_wave()
{
    if(zitai_count>=100)
    {
        clear_data();
    }
    if(controlflag==4)
        setupQuadraticDemo(ui->customPlot);
        //ui->customPlot->replot();
}
void MainWindow::delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::read_gameController()
{
    unsigned char buf[8];
    res = hid_read(handle, buf, 8);
    for (int i = 0; i < res; i++)
    {
        //qDebug("buf[%d]: %x  ", i, buf[i]);
        gamedata[i] = buf[i];
    }

    //清屏
    ui->game_data->clear();
    //刷新数据
    QString str;
    str = QString("[1]:%1 \n[2]:%2 \n[3]:%3 \n[4]:%4 \n[5]:%5 \n[6]:%6 \n[7]:%7 \n[8]:%8").arg(gamedata[0]).arg(gamedata[1]).arg(gamedata[2]).arg(gamedata[3]).arg(gamedata[4]).arg(gamedata[5]).arg(gamedata[6]).arg(gamedata[7]);
    ui->game_data->append(str);
    if (controlflag==2){
        if(gamedata[0]==0 && last_game[0]==127){
            serial_port_t->write("G91 x1\r\n");
            //ui->send_text->clear();
            ui->send_text->setText("G91 x1\r\n");
            //delay(150);
            Arm_x+=1;
        }
        if(gamedata[0]==255 && last_game[0]==127){
            serial_port_t->write("G91 x-1\r\n");
            //ui->send_text->clear();
            ui->send_text->setText("G91 x-1\r\n");
            //delay(150);
            Arm_x-=1;
        }
        if(gamedata[1]==0 && last_game[1]==127){
            serial_port_t->write("G91 z0.5\r\n");
            //ui->send_text->clear();
            ui->send_text->setText("G91 z0.5\r\n");
            //delay(150);
            Arm_z+=0.5;
         }
        if(gamedata[1]==255 && last_game[1]==127){
            serial_port_t->write("G91 z-0.5\r\n");
            //ui->send_text->clear();
            ui->send_text->setText("G91 z-0.5\r\n");
            //delay(150);
            Arm_z-=0.5;
        }
        if(gamedata[5]==31 && last_game[5]==15){
            serial_port_t->write("G91 y1\r\n");
            //ui->send_text->clear();
            ui->send_text->setText("G91 y1\r\n");
            //delay(150);
            Arm_y+=1;
        }
        if(gamedata[5]==79 && last_game[5]==15){
            serial_port_t->write("G91 y-1\r\n");
            //ui->send_text->clear();
            ui->send_text->setText("G91 y-1\r\n");
            //delay(150);
            Arm_y-=1;
        }
        if(gamedata[5]==47 && last_game[5]==15){
            serial_port_t->write("G91 B1\r\n");//向右平移
            //ui->send_text->clear();
            ui->send_text->setText("G91 B1\r\n");
            //delay(150);
            Arm_u+=1;
        }
        if(gamedata[5]==143 && last_game[5]==15){
            serial_port_t->write("G91 B-1\r\n"); //向左平移
            //ui->send_text->clear();
            ui->send_text->setText("G91 B-1\r\n");
            //delay(150);
            Arm_u-=1;
        }
    }
    for (int i = 0; i < res; i++)
    {

        last_game[i] = gamedata[i];
    }

    showdata();
}
//画窗体
void MainWindow::setupQuadraticDemo(QCustomPlot *customPlot)
{
  //每条曲线都会独占一个graph()
  customPlot->addGraph();
  customPlot->graph(0)->setPen(QPen(Qt::blue)); // 曲线的颜色
  customPlot->addGraph();//添加graph等价于添加新曲线
  customPlot->graph(1)->setPen(QPen(Qt::red)); // 曲线的颜色
  customPlot->addGraph();//添加graph等价于添加新曲线
  customPlot->graph(2)->setPen(QPen(Qt::black)); // 曲线的颜色

  // 边框右侧和上侧均显示刻度线，但不显示刻度值:
  // (参见 QCPAxisRect::setupFullAxesBox for a quicker method to do this)
  customPlot->xAxis2->setVisible(true);
  customPlot->xAxis2->setTickLabels(false);
  customPlot->yAxis2->setVisible(true);
  customPlot->yAxis2->setTickLabels(false);

  customPlot->graph(0)->rescaleAxes(true);
  //自动调整XY轴的范围，以便显示出graph(1)中所有的点
  customPlot->graph(1)->rescaleAxes(true);
  //自动调整XY轴的范围，以便显示出graph(2)中所有的点
  customPlot->graph(2)->rescaleAxes(true);

  // 使上下两个X轴的范围总是相等，使左右两个Y轴的范围总是相等
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
  // 把已存在的数据填充进graph的数据区
  customPlot->graph(0)->setData(time_count, x);
  customPlot->graph(1)->setData(time_count, y);
  customPlot->graph(2)->setData(time_count, z);
  // 支持鼠标拖拽轴的范围、滚动缩放轴的范围，左键点选图层（每条曲线独占一个图层）
  customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
  // 立即刷新图像
  ui->customPlot->replot();
}

//打开串口
void MainWindow::on_open_clicked()
{
    /////////////机械臂串口///////////////
    if(m_bIsOpenPort == false)      //打开串口
    {
        //指定串口信息（串口名称）
        QSerialPortInfo *port_info = new QSerialPortInfo(ui->com->currentText());
        //绑定串口
        serial_port_t->setPort(*port_info);

        baud_activated(ui->baud->currentText()); //设置波特率
        jiaoyan_activated(ui->jiaoyan->currentIndex()); //设置奇偶校验
        date_activated(ui->date->currentText()); //设置数据位
        stop_activated(ui->stop->currentText()); //设置停止位
        if(ui->checkBox_send->isChecked())
            hex_send=true;
        else
            hex_send=false;
        if(ui->checkBox_receive->isChecked())
            hex_receive=true;
        else
            hex_receive=false;
        //打开串口
        if(!serial_port_t->open(QIODevice::ReadWrite))
        {
            QMessageBox::warning(this,"警告","打开串口失败！");
            return;
        }

        ui->open->setText(tr("关闭串口"));
        m_bIsOpenPort = true;
        ui->progressBar->setValue(100);
    }
    else                    //关闭串口
    {
        ReturnFlag=0;
        serial_port_t->close();
        myserial.close();
        ui->open->setText(tr("打开串口"));
        m_bIsOpenPort = false;
        ui->progressBar->setValue(0);
    }
    ///////////////另外一个串口////////////////
    if (controlflag==3||controlflag==4||controlflag==5||controlflag==6){
        myserial.setPortName(ui->comboBox->currentText()); //设置串口号
        myserial.setBaudRate(115200); //设置波特率
        myserial.setDataBits(QSerialPort::Data8); //数据位
        myserial.setFlowControl(QSerialPort::NoFlowControl);
        myserial.setParity(QSerialPort::NoParity);
        myserial.setStopBits(QSerialPort::OneStop);
        myserial.close();
        if(myserial.open(QIODevice::ReadWrite))
        {
            connect(&myserial,SIGNAL(readyRead()),this,SLOT(read_COM()));
            //ui->progressBar->setValue(100);
           //将串口的 readyread 信号绑定到 read_com 这个槽函数上
        }
        if(controlflag==4)
        {
            hex_receive = true;
            QTimer *waveshow_timer = new QTimer(this);
            connect(waveshow_timer, SIGNAL(timeout()), this, SLOT(show_wave()));
            waveshow_timer->start(100); // 每隔0.01s

            QTimer *timer_send = new QTimer(this);
            connect(timer_send, SIGNAL(timeout()), this, SLOT(send_zitai_data()));
            timer_send->start(500); // 每隔0.5s
            //显示窗体
            setupQuadraticDemo(ui->customPlot);
        }
        if(controlflag==6)
        {
            //QTimer *timer = new QTimer(this);
            //connect(timer, SIGNAL(timeout()), this, SLOT(Water_timeEvent()));
            //timer->start(500); // 每隔0.01s
        }

        else
            hex_receive = false;
    }
    if(controlflag==2)
    {
        //打开hid设备,输入id
        handle = hid_open(0x081F, 0xE401, NULL);
        //添加手柄扫描10ms一次
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(read_gameController()));
        timer->start(5); // 每隔0.01s
    }
}
void MainWindow::clear_data()
{
    //数据清零//
    x.clear();
    y.clear();
    z.clear();
    time_count.clear();
    x.resize(1);
    y.resize(1);
    z.resize(1);
    time_count.resize(1);
    //内存清零//
    x.capacity();
    y.capacity();
    z.capacity();
    time_count.capacity();
    zitai_count=1;
}
////////////////刷新角度数据//////////////////
void MainWindow::showdata()
{
    ui->lcdNumber_x->display(Arm_x);
    ui->lcdNumber_y->display(Arm_y);
    ui->lcdNumber_z->display(Arm_z);
    ui->lcdNumber_u->display(Arm_u);
    ui->horizontalSlider_x->setValue(50+50*Arm_x/12);
    ui->horizontalSlider_y->setValue(100*Arm_y/16);
    ui->horizontalSlider_z->setValue(100*Arm_z/18);
    ui->horizontalSlider_u->setValue(50+50*Arm_u/10);
}
//串口2读取
void MainWindow::read_COM()
{
    QByteArray mytemp = myserial.readAll();
     if(!mytemp.isEmpty())
     {
         /////////////
         //串口2读取
         if(controlflag==5)//蓝牙
               {
                   QString mytemp_1 = mytemp.mid(2,1);
                   //qDebug() <<"蓝牙:" <<mytemp_1<< endl;
                   //ui->textEdit->append(mytemp);
                   //qDebug() << mytemp << endl;
                   if(ReturnFlag==1){
                       switch (mytemp_1.toInt())
                      {
                          case 1:
                              serial_port_t->write("G91 z2\r\n");
                              //ui->textEdit->append(mytemp);
                              ui->textEdit->setText("向上");
                              ui->send_text->setText("G91 z2\r\n");
                              Arm_z+=2;
                              break;
                          case 3:
                              serial_port_t->write("G91 z-2\r\n");
                              ui->textEdit->setText("向下");
                              ui->send_text->setText("G91 z2-\r\n");
                              Arm_z-=2;
                              break;
                          case 2:
                              serial_port_t->write("G91 x3\r\n");
                              ui->textEdit->setText("向左");
                              ui->send_text->setText("G91 x3\r\n");
                              Arm_x+=3;
                              break;
                          case 4:
                              serial_port_t->write("G91 x-3\r\n");
                              ui->textEdit->setText("向右");
                              ui->send_text->setText("G91 x-3\r\n");
                              Arm_x-=3;
                              break;

                          default:
                          break;
                      }
                   }
              }

         /////////////
         else if(controlflag==4){
             //ui->textEdit->append(mytemp.toHex());
             //qDebug() << mytemp.toHex() << endl;
             QString zitai = mytemp.toHex();

             QString RollL = zitai.mid(4,2);
             //ui->textEdit->append(RollL);
             QString RollH = zitai.mid(6,2);
             QString PitchL = zitai.mid(8,2);
             QString PitchH = zitai.mid(10,2);
             QString YawL = zitai.mid(12,2);
             QString YawH = zitai.mid(14,2);
             QString TL = zitai.mid(16,2);
             QString TH = zitai.mid(18,2);
             QString SUM = zitai.mid(20,2);
             bool ok_1,ok_2,ok_3,ok_4;

             x_data = float((RollH.toInt(&ok_1,16)<<8)|RollL.toInt(&ok_1,16))/32768.0*180.0 - 180.0;
             y_data = float((PitchH.toInt(&ok_2,16)<<8)|PitchL.toInt(&ok_2,16))/32768.0*180.0 - 180.0;
             z_data = float((YawH.toInt(&ok_3,16)<<8)|YawL.toInt(&ok_3,16))/32768.0*180.0 - 180.0;
             Temper = float((TH.toInt(&ok_4,16)<<8)|TL.toInt(&ok_4,16))/100.0 - 118.0;
             Arm_x=floor((x_data*10)/36)/10;
             Arm_y=0;
             Arm_z=floor((z_data*5)/36)/10+2.5;
             Arm_u=floor((y_data*10)/36)/10;
//             kal_x = Kalman_Filter(Kalman_x,x_data);
//             kal_y = Kalman_Filter(Kalman_y,y_data);
//             kal_z = Kalman_Filter(Kalman_z,z_data);
             //qDebug() << "jiao:"<< x_data << endl;
             ui->lcd_x->display(x_data);
             ui->lcd_y->display(y_data);
             ui->lcd_z->display(z_data);
             ui->lcd_temper->display(Temper);
             ui->lcd_temper->setSegmentStyle(QLCDNumber::Flat);

             QPalette lcdpat = ui->lcd_temper->palette();

             //////////////////////////////////////
             QString send_x;
             send_x = QString("G90 x%1 y0 z%2 a0 b%3\r\n").arg(floor((x_data*10)/36)/10).arg(floor((z_data*5)/36)/10+2.5).arg(floor((y_data*10)/36)/10);
             ba = send_x.toLatin1(); // must
             // 生成模拟数据点
             x.push_back(x_data);
             y.push_back(y_data);
             z.push_back(z_data);
             time_count.push_back(zitai_count);

//             ui->customPlot->graph(0)->addData(time_count[zitai_count], x[zitai_count]);
//             ui->customPlot->graph(1)->addData(time_count[zitai_count], y[zitai_count]);
//             ui->customPlot->graph(2)->addData(time_count[zitai_count], z[zitai_count]);
             zitai_count++;
         }
         //正常接收
         else if(controlflag==3)
                  {
                      qDebug() << "yuying:"<<mytemp << endl;
                      if(mytemp=="111\r\n")
                          ui->textEdit->setText("z轴向上");
                      else if(mytemp=="222\r\n")
                          ui->textEdit->setText("z轴向下");
                      else if(mytemp=="333\r\n")
                          ui->textEdit->setText("x轴向左");
                      else if(mytemp=="444\r\n")
                          ui->textEdit->setText("x轴向右");
                      else if(mytemp=="555\r\n")
                          ui->textEdit->setText("滑台向左");
                      else if(mytemp=="666\r\n")
                          ui->textEdit->setText("滑台向右");
                      else if(mytemp=="999\r\n")//过来
                      {
                          recognize(999);
                          ui->textEdit->setText("过来");
                      }
                      else if(mytemp=="100\r\n")//滚
                      {
                          recognize(100);
                          ui->textEdit->setText("滚");
                      }
                      else if(mytemp=="101\r\n")//回去
                      {
                          recognize(101);
                          ui->textEdit->setText("回去");     
                      }
                      else if(mytemp=="102\r\n")//识别
                      {
                          recognize(102);
                          ui->textEdit->setText("识别");
                      }
                      else
                          ui->textEdit->setText(mytemp);
                      switch (mytemp.toInt())
                      {
                          case 111:
                              serial_port_t->write("G91 z2\r\n");
                              ui->send_text->setText("G91 z2\r\n");
                              Arm_z+=2;
                          break;
                          case 222:
                              serial_port_t->write("G91 z-2\r\n");
                              ui->send_text->setText("G91 z-2\r\n");
                              Arm_z-=2;
                              break;
                          case 333:
                              serial_port_t->write("G91 x3\r\n");
                              ui->send_text->setText("G91 x3\r\n");
                              Arm_x+=3;
                              break;
                          case 444:
                              serial_port_t->write("G91 x-3\r\n");
                              ui->send_text->setText("G91 x-3\r\n");
                              Arm_x-=3;
                              break;
                          case 555:
                              serial_port_t->write("G91 b3\r\n");
                              ui->send_text->setText("G91 b3\r\n");
                              Arm_u+=3;
                              break;
                          case 666:
                              serial_port_t->write("G91 b-3\r\n");
                              ui->send_text->setText("G91 b-3\r\n");
                              Arm_u-=3;
                              break;
                          case 999://过来
                              target.x = P_C.x;
                              target.y = P_org.y;
                              target.z = P_C.z;
                              coordinate(&target);
                              break;
                          case 100://滚
                              target.x = P_org.x;
                              target.y = P_C.y;
                              target.z = P_org.z;
                              coordinate(&target);
                              break;
                          case 101://回去
                              target.x = P_org.x;
                              target.y = P_C.y;
                              target.z = P_org.z;
                              coordinate(&target);
                              break;
                          case 102://识别
                              break;
                          default:
                          break;
                      }
                  }
         else if(controlflag==6)//流水线串口
                  {     
                        int now_step = mytemp.toInt();
                        qDebug() <<"收到 : "<< now_step << endl;
                        qDebug() <<"状态 : "<< flag_step1 << endl;
                        if(mytemp=="1\r\n"&&flag_step1==0){
                            qDebug() << "Stm32:检测到物块,停止传送带" << endl;
                            flag_step1=1;
                            coordinate(&P_CatchUp);
                            coordinate(&P_CatchDown);
                            qDebug() << "QT:伸出机械臂到物块上方" << endl;
                            myserial.write("1\r\n");
                        }
                        else if(mytemp=="2\r\n"&&flag_step1==1){
                            qDebug() << "Stm32:开启气泵" << endl;
                            coordinate(&P_CatchSky);
                            flag_step1=2;
                            myserial.write("2\r\n");
                            qDebug() << "QT:抬起机械臂" << endl;
                        }
                        else if(mytemp=="3\r\n"&&flag_step1==2){
                            qDebug() << "Stm32:空" << endl;
                            serial_port_t->write("G91 X-6.5\r\n");
                            delay(100*3);
                            myserial.write("3\r\n");
                            qDebug() << "QT:向左旋转机械臂" << endl;
                            flag_step1=3;
                        }
                        else if(mytemp=="4\r\n"&&flag_step1==3){
                            qDebug() << "Stm32:打闭电磁阀" << endl;
                            delay(100);
                            serial_port_t->write("G91 X6.5\r\n");
                            delay(100*3);
                            myserial.write("4\r\n");
                            flag_step1=4;
                            qDebug() << "QT:向右旋转机械臂" << endl;
                        }
                        else if(mytemp=="5\r\n"&&flag_step1==4){
                            qDebug() << "Stm32:关闭气泵" << endl;
                            coordinate(&P_org);
                            myserial.write("9\r\n");
                            flag_step1=0;
                            qDebug() << "w.P_org" << endl;
                            qDebug() << "QT:机械臂回原点" << endl;
                        }


                        if(last_step!=now_step) last_step = now_step;
                  }

         else{

             //十六进制接收
             if(hex_receive){
                 ui->textEdit->setText(mytemp.toHex());
                 //qDebug() << buf.toHex() << endl;
             }
             //正常接收
             else{
                 ui->textEdit->setText(mytemp);
                 //qDebug() << buf << endl;
             }
         }
         showdata();
     }
}
//清除回读信息区域
void MainWindow::on_clear_clicked()
{
    ui->recv_text->clear();
}

//串口发送数据
void MainWindow::on_send_clicked()
{
    QString q = ui->send_text->toPlainText();
    QByteArray f = q.toLatin1();
    char *temp = f.data();
    //十六进制发送
    if(hex_send)
        serial_port_t->write(QByteArray::fromHex(q.toLatin1()));
    //正常发送
    else
        serial_port_t->write(temp);
}

//接收回读串口数据
void MainWindow::read_date()
{
    QByteArray buf;
    buf = serial_port_t->readAll();

    //十六进制接收
    if(hex_receive){
        ui->recv_text->append(buf.toHex());
        //qDebug() << buf.toHex() << endl;
    }
    //正常接收
    else{
        ui->recv_text->append(buf);
        //qDebug() << buf << endl;
    }
}

//设置波特率
void MainWindow::baud_activated(const QString &arg1)
{
    switch(arg1.toInt())
    {
    case 1200:
        serial_port_t->setBaudRate(QSerialPort::Baud1200);
        break;
    case 2400:
        serial_port_t->setBaudRate(QSerialPort::Baud2400);
        break;
    case 9600:
        serial_port_t->setBaudRate(QSerialPort::Baud9600);
        break;
    case 38400:
        serial_port_t->setBaudRate(QSerialPort::Baud38400);
        break;
    case 115200:
        serial_port_t->setBaudRate(QSerialPort::Baud115200);
        break;
    }
}

//设置奇偶校验
void MainWindow::jiaoyan_activated(int arg1)
{
    switch(arg1)
    {
    case 0:
        serial_port_t->setParity(QSerialPort::NoParity);
        break;
    case 1:
        serial_port_t->setParity(QSerialPort::EvenParity);
        break;
    case 2:
        serial_port_t->setParity(QSerialPort::OddParity);
        break;
    }
}

//设置数据位
void MainWindow::date_activated(const QString &arg1)
{
    switch(arg1.toInt())
    {
    case 5:
        serial_port_t->setDataBits(QSerialPort::Data5);
        break;
    case 6:
        serial_port_t->setDataBits(QSerialPort::Data6);
        break;
    case 7:
        serial_port_t->setDataBits(QSerialPort::Data7);
        break;
    case 8:
        serial_port_t->setDataBits(QSerialPort::Data8);
        break;
    }
}

//设置停止位
void MainWindow::stop_activated(const QString &arg1)
{
    switch(arg1.toInt())
    {
    case 1:
        serial_port_t->setStopBits(QSerialPort::OneStop);
        break;
    case 2:
        serial_port_t->setStopBits(QSerialPort::TwoStop);
        break;
    }
}




void MainWindow::on_checkBox_send_stateChanged()
{
    if(ui->checkBox_send->isChecked())
        hex_send=true;
    else
        hex_send=false;
}

void MainWindow::on_checkBox_receive_stateChanged()
{
    if(ui->checkBox_receive->isChecked())
        hex_receive=true;
    else
        hex_receive=false;
}
////////////////////////////////////////////

//选择控制方向，确定标志位
void MainWindow::on_control_activated()
{
    if(ui->control->currentText()=="键盘"){
        controlflag=1;
        ui->label_13->setText("键盘控制");
        ui->comboBox->setCurrentText("-No-");
    }
    else if(ui->control->currentText()=="手柄"){
        controlflag=2;
        ui->label_13->setText("手柄控制");
        ui->comboBox->setCurrentText("-No-");
    }
    else if(ui->control->currentText()=="语音"){
        ui->label_13->setText("语音串口");
        controlflag=3;
        ui->comboBox->setCurrentText("COM22");
    }
    else if(ui->control->currentText()=="姿态"){
        ui->label_13->setText("姿态串口");
        controlflag=4;
        ui->comboBox->setCurrentText("COM4");
    }
    else if(ui->control->currentText()=="蓝牙"){
        ui->label_13->setText("蓝牙串口");
        controlflag=5;
        ui->comboBox->setCurrentText("COM4");
    }
    else if(ui->control->currentText()=="流水线"){
        ui->label_13->setText("控制器串口");
        controlflag=6;
        ui->comboBox->setCurrentText("COM6");
    }

}


//键盘控制
void MainWindow::on_pushButton_up_clicked()
{
    if(controlflag==1)
    serial_port_t->write("G91 z1\r\n");

    ui->send_text->setText("G91 z1\r\n");
    Arm_z+=1;
    showdata();
}

void MainWindow::on_pushButton_right_clicked()
{
    if(controlflag==1)
    serial_port_t->write("G91 x-0.5\r\n");
    ui->send_text->setText("G91 x-0.5\r\n");
    Arm_x-=0.5;
    showdata();
}

void MainWindow::on_pushButton_left_clicked()
{
    if(controlflag==1)
    serial_port_t->write("G91 x0.5\r\n");
    ui->send_text->setText("G91 x0.5\r\n");
    Arm_x+=0.5;
    showdata();
}

void MainWindow::on_pushButton_down_clicked()
{
    if(controlflag==1)
    serial_port_t->write("G91 z-1\r\n");
    ui->send_text->setText("G91 z-1\r\n");
    Arm_z-=1;
    showdata();
}

void MainWindow::on_pushButton_Rect_clicked()
{ 

    target.x = -3.5-5;
    target.y = 0;
    target.z = 8;
    coordinate(&target);
    //delay(200);
    target.x = -3.5-5;
    target.y = -4;
    target.z = 8;
    coordinate(&target);
    target.x = -3.5;
    target.y = -4;
    target.z = 8;
    coordinate(&target);
    target.x = -3.5;
    target.y = 0;
    target.z = 8;
    coordinate(&target);
//    target = {-3.5,-3,8};
//    coordinate(&target);
//    target = {-3.5,0,8};
//    coordinate(&target);
}

void MainWindow::on_pushButton_ll_clicked()
{
    if(controlflag==1)
    serial_port_t->write("G91 b+1\r\n");
    ui->send_text->setText("G91 b+1\r\n");
    Arm_u+=1;
    showdata();
}

void MainWindow::on_pushButton_rr_clicked()
{
    if(controlflag==1)
    serial_port_t->write("G91 b-1\r\n");
    ui->send_text->setText("G91 b-1\r\n");
    Arm_u-=1;
    showdata();
}

//归位
void MainWindow::on_pushButton_return_clicked()
{ 
    ReturnFlag=0;
    Mytime=10;
    serial_port_t->write("$H\r\n"); //归位
    serial_port_t->write("G91 x7.4\r\n"); //x轴归位
    serial_port_t->write("G91 z-4.5\r\n");//z轴归位
    serial_port_t->write("G92 X0 Y0 Z0 A0 B0\r\n");//设置原点
    Arm_x=-3.5;
    Arm_y=0;
    Arm_z=8;
    Arm_u=0;
    QTimer *time_wait = new QTimer(this);
    connect(time_wait, SIGNAL(timeout()), this, SLOT(timeEvent()));
    time_wait->start(1000); // 每隔1s
    showdata();
    P_now.x = -3.5;
    P_now.y = 0;
    P_now.z = 8;
}
void MainWindow::timeEvent()
{
    Mytime =Mytime - 1;

    if(Mytime<=0)
    {
        ReturnFlag=1;  
    }
    else
      qDebug() << "time:"<< Mytime << endl;
}

void MainWindow::on_pushButton_clear_clicked()
{
    clear_data();
}



void MainWindow::on_pushButton_database_clicked()
{
    mymodel->setTable("HugeSmart_ARM");
    mymodel->select();
}
void MainWindow::loadNavigate()
{
  QString sUrl = lineUrl->text().trimmed();
  webWidget->dynamicCall("Navigate(const QString&)",sUrl);
}
void MainWindow::on_pushButton_add_clicked()
{
    QDateTime mycurrenttime = QDateTime::currentDateTime();
    QString myTimestr = mycurrenttime.toString("yyyy-MM-dd hh:mm:ss");
    int rowNum = mymodel->rowCount(); //获得表的行数
    int id = rowNum+1;
    mymodel->insertRow(rowNum); //添加一行
    qDebug() << id << endl;
    QString input_data = ui->send_text->toPlainText();
    mymodel->setData(mymodel->index(rowNum,0),id);
    mymodel->setData(mymodel->index(rowNum,1),input_data);
    mymodel->setData(mymodel->index(rowNum,2),myTimestr);
    mymodel->submitAll();
    //id +=1;
}

void MainWindow::on_pushButton_del_clicked()
{
    int curRow = ui->tableView->currentIndex().row();
    mymodel->removeRow(curRow);
    int ok = QMessageBox::warning(this,tr("删除当前行!"),tr("你确定""删除当前行吗？"),QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
    {
        mymodel->revertAll(); //如果不删除，则撤销
    }
    else mymodel->submitAll(); //否则提交，在数据库中删除该行
}

void MainWindow::on_pushButton_again_clicked()
{

    QSqlQuery query;

    // 输出整张表
    query.exec("select * from HugeSmart_ARM");
    while(query.next()){
         qDebug() << query.value(1).toString();
         QString sql_temp = query.value(1).toString();
         QByteArray sql_array = sql_temp.toLatin1();
         serial_port_t->write(sql_array);
    }

}
void MainWindow::connected()
{
    QMessageBox::about(this,"提示","连接成功");
    connect(this->socket,SIGNAL(readyRead()),this,SLOT(readyread()));
}
void MainWindow::readyread()
{
     QMessageBox::about(this,"提示","准备读取");
     QByteArray arr=this->socket->readAll();
     QDataStream * dst=new
     QDataStream(&arr,QIODevice::ReadOnly);/******重点******/
     QString str1;
     QString str2;
     (*dst)>>str1>>str2;
     //this->ui-> messagetextBrowser->append(str1+str2);
     //this->ui->clienttextBrowser->setText("\n");
     QMessageBox::about(this,"x",str1+str2);
}

//   float x0;
//   float y0;//机械臂末端当前位置

void MainWindow::hannuo(int n,char one ,char two,char three)
{
      if(n==1)move(one, three); //递归截止条件
      else
    {
      hannuo(n-1,one ,three,two);//将 n-1个盘子先放到B座位上
      move(one,three);//将A座上地剩下的一个盘移动到C盘上
      hannuo(n-1,two,one,three);//将n-1个盘从B座移动到C座上

    }
}
void MainWindow::move(char x,char y)//将圆环从a移动到b
{
 printf("%c--->%c\r\n",x,y);
 if(x=='A'){
    coordinate(&P_A);//移动到柱A的上方
    target.x=P_A.x;
    target.y=P_A.y;
    target.z=h_A;
    coordinate(&target);//移动到所需夹取圆环处
//	z0=h_A;
    h_A-=0.7;
    /////////完成夹取

    serial_port_t->write("M3 S15\r\n");
    delay(500);
    coordinate(&P_A);//移动到柱A的上方
    if(y=='B'){
        coordinate(&P_B);//移动到柱B的上方
        target.x=P_B.x;
        target.y=P_B.y;
        target.z=h_B;
        coordinate(&target);//移动到所需放置圆环处
        /////////放下圆柱

        serial_port_t->write("M3 S40\r\n");
        delay(500);
        coordinate(&P_B);
        h_B+=0.7;
    }
    else if(y=='C'){
        coordinate(&P_C);//移动到柱C的上方
        target.x=P_C.x;
        target.y=P_C.y;
        target.z=h_C;
        coordinate(&target);//移动到所需放置圆环处
        /////////放下圆柱

        serial_port_t->write("M3 S40\r\n");
        delay(500);
        coordinate(&P_C);
        h_C+=0.7;
    }
 }
 if(x=='B'){
    coordinate(&P_B);//移动到柱B的上方
    target.x=P_B.x;
    target.y=P_B.y;
    target.z=h_B;
    coordinate(&target);//移动到所需夹取圆环处
    h_B-=0.7;
    /////////完成夹取


    serial_port_t->write("M3 S15\r\n");
    delay(500);
    coordinate(&P_B);//移动到柱B的上方
    if(y=='A'){
        coordinate(&P_A);//移动到柱A的上方
        target.x=P_A.x;
        target.y=P_A.y;
        target.z=h_A;
        coordinate(&target);//移动到所需夹取圆环处
        /////////放下圆柱

        serial_port_t->write("M3 S40\r\n");
        delay(500);
        coordinate(&P_A);
        h_A+=0.7;
    }
    else if(y=='C'){
        coordinate(&P_C);//移动到柱C的上方
        target.x=P_C.x;
        target.y=P_C.y;
        target.z=h_C;
        coordinate(&target);//移动到所需放置圆环处

        serial_port_t->write("M3 S40\r\n");
        delay(500);
        coordinate(&P_C);
        h_C+=0.7;
    }
 }
 if(x=='C'){
    coordinate(&P_C);//移动到柱C的上方
    target.x=P_C.x;
    target.y=P_C.y;
    target.z=h_C;
    coordinate(&target);//移动到所需夹取圆环处
//	z0=h_C;
    h_C-=0.7;

    serial_port_t->write("M3 S15\r\n");
    delay(500);
    /////////完成夹取
    coordinate(&P_C);//移动到柱C的上方

    if(y=='A'){
        coordinate(&P_A);//移动到柱A的上方
        target.x=P_A.x;
        target.y=P_A.y;
        target.z=h_A;

        coordinate(&target);//移动到所需夹取圆环处

        serial_port_t->write("M3 S40\r\n");
        delay(500);
        /////////放下圆柱
        coordinate(&P_A);
        h_A+=0.7;
    }
    else if(y=='B'){
        coordinate(&P_B);//移动到柱B的上方
        target.x=P_B.x;
        target.y=P_B.y;
        target.z=h_B;
        coordinate(&target);//移动到所需夹取圆环处
        /////////放下圆柱

        serial_port_t->write("M3 S40\r\n");
        delay(500);
        coordinate(&P_B);
        h_B+=0.7;
    }
 }
}
void MainWindow::on_PushButton_hannuo_clicked()
{

    QString str=ui->lineEdit_hannuo->text();
    int num = str.toInt();
    serial_port_t->write("M3 S40\r\n");
    h_A=2.1+0.7*num;
    h_B=2.1;
    h_C=2.1;
    hannuo(num,'A','B','C');
    coordinate(&P_A);//移动到柱A的上方
}

void MainWindow::recognize(int num)
{
    QFile f("P:\\Item\\QT\\ARM_Controller\\temp.txt");
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()  << "Open failed." << endl;
    }

    QTextStream txtOutput(&f);
    QString s1 =  QString::number(num,15);
    qDebug() << s1<< endl;
    txtOutput << s1 << endl;

    f.close();
}

void MainWindow::on_pushButton_clicked()
{

    QString str_x=ui->lineEdit_targetX->text();
    float x = str_x.toFloat();
    QString str_y=ui->lineEdit_targetY->text();
    float y = str_y.toFloat();
    QString str_z=ui->lineEdit_targetZ->text();
    float z = str_z.toFloat();
    QString str_q=ui->lineEdit_targetQ->text();
    float q = str_q.toFloat();
    _POINT_ tarP;
    tarP.x = x;
    tarP.y = y;
    tarP.z = z;
    tarP.q = q;
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);
}



void MainWindow::coordinate(_POINT_* tar)//从(x1,y1)移动到(x2,y2)
{
    finish_flag=0;
    float faiz;
    float theta_1;
    float theta_2;
    float move_z;
    float move_y;
    float move_B;
    float q=tar->q;
//////////
    float x=P_now.x;
    float y=P_now.y;
    float z=P_now.z;

    int x_num = fabs(x-tar->x)/0.5;
    int y_num = fabs(y-tar->y)/0.5;
    int z_num = fabs(z-tar->z)/0.5;
    int max_num = MAX(x_num,y_num,z_num);

    for(int i = 0;i <= max_num;i++)
    {
        if(i!=0){
            x=x+0.5*judge(x,tar->x);//用于判断向x轴正向或负向移动
            y=y+0.5*judge(y,tar->y);//用于判断向y轴正向或负向移动
            z=z+0.5*judge(z,tar->z);//用于判断向z轴正向或负向移动
        }
       faiz    = acos(-(l2*l2-(x*x+z*z)-l1*l1)/(2*l1*sqrt(x*x+z*z)));
       theta_1 = 180.0*(atan2(z,x)-faiz)/M_PI;
       theta_2 = 180.0*(acos((x*x+z*z-l1*l1-l2*l2)/(2*l1*l2)))/M_PI;
       if(i<=x_num||i<=z_num){
           move_z = (theta_1-45.0)/6.3;//机械臂z轴相对坐标
           move_y = (theta_1 + theta_2-182.3)/6.3;//机械臂y轴相对坐标
       }
       move_B = (y*3.125);

       QString send_x;
       send_x = QString("G90 x%4 y%1 z%2 a0 b%3\r\n").arg(floor(move_y*100)/100).arg(floor(move_z*100)/100).arg(floor(move_B*100)/100).arg(floor(q*100)/100);
       sendarray = send_x.toLatin1();
       serial_port_t->write(sendarray);

       //qDebug() << "RECT:"<< send_x << endl;
    }
    P_now.x = x;
    P_now.y = y;
    P_now.z = z;
    //qDebug() << "NUM: x="<< x_num<<"  y="<<y_num<<"  z="<<z_num << endl;
    //qDebug() << "NOW xyz:"<< P_now.x<<"  "<<P_now.y<<"  "<<P_now.z << endl;
    delay(max_num*150);
    finish_flag=1;
}

void MainWindow::Water_timeEvent(){
    if(flag_step1==1){
        coordinate(&P_CatchUp);
        coordinate(&P_CatchDown);
        qDebug() << "伸出机械臂到物块上方" << endl;
        myserial.write("1\r\n");
    }
    else if(flag_step1==2){
        coordinate(&P_CatchSky);
        flag_step1=3;
        myserial.write("2\r\n");
        qDebug() << "抬起机械臂" << endl;
    }
    else if(flag_step1==3){
        serial_port_t->write("G91 X-6.5\r\n");
        delay(800);
        myserial.write("3\r\n");
        qDebug() << "G91 X-6.5" << endl;
    }
    else if(flag_step1==4){
        serial_port_t->write("G91 X6.5\r\n");
        delay(800);
        myserial.write("4\r\n");
        qDebug() << "G91 X6.5" << endl;
    }
    else if(flag_step1==5){
        coordinate(&P_org);
        qDebug() << "w.P_org" << endl;
        flag_step1 = 0;
        myserial.write("9\r\n");
    }
}
////////////////////

void MainWindow::qiB_check()
{
    if(signal_qiB==0){
        myserial.write("5\r\n");
        signal_qiB=1;
        this->ui->pushButton_qiB->setText("气泵已打开");
    }
    else if(signal_qiB==1){
        myserial.write("6\r\n");
        signal_qiB=0;
        this->ui->pushButton_qiB->setText("气泵已关闭");
    }
}

void MainWindow::elec_check()
{
    if(signal_ele==0){
        myserial.write("3\r\n");
        signal_ele=1;
        this->ui->pushButton_elec->setText("电磁阀已打开");
    }
    else if(signal_ele==1){
        myserial.write("4\r\n");
        signal_ele=0;
        this->ui->pushButton_elec->setText("电磁阀已关闭");
    }
}

void MainWindow::dilver_check()
{
    if(signal_deliver==0){
        myserial.write("1\r\n");
        signal_deliver=1;
        this->ui->pushButton_dilver->setText("传送带已打开");
    }
    else {
        myserial.write("2\r\n");
        this->ui->pushButton_dilver->setText("传送带已关闭");
        signal_deliver=0;
    }

}
//////////////////////
void MainWindow::on_pushButton_qiB_clicked()
{
   qiB_check();
}

void MainWindow::on_pushButton_elec_clicked()
{
    elec_check();
}

void MainWindow::on_pushButton_dilver_clicked()
{
    dilver_check();
}
void MainWindow::chase()
{
    QFile file("P:\\Item\\QT\\ARM_Controller\\temp.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }

    QByteArray line = file.readLine();
    QString str(line);
    qDebug()<< str.length()<<endl;
    QStringList xyz = str.split(".), tensor(");
    int indexOfEth0= xyz.at(0).indexOf("or(");
    int leftup_data_x = xyz.at(0).mid(indexOfEth0+3).toDouble();
    int leftup_data_y = xyz.at(1).toDouble();
    int rightdown_data_x = xyz.at(2).toDouble();
    int indexOfEth1= xyz.at(3).indexOf("or(");
    int indexOfEth2= xyz.at(3).indexOf(".)]");
    int rightdown_data_y = xyz.at(3).mid(indexOfEth1,indexOfEth2-indexOfEth1).toDouble();;


        qDebug()<< str<<endl;
        qDebug()<<"lu_x"<< leftup_data_x<<endl;
        qDebug()<<"lu_y"<< leftup_data_y<<endl;
        qDebug()<<"rd_x"<< rightdown_data_x<<endl;
        qDebug()<<"rd_y"<< rightdown_data_y<<endl;
    float temp_c = 0.1986f*((leftup_data_y+rightdown_data_y)/2)+99.38f;
    float temp_r = 0.2034f*((leftup_data_x+rightdown_data_x)/2)+118.3f;
    float temp_q = 8.494f*(temp_c/temp_r)-1.568f;
    float temp_xie = sqrt(temp_c*temp_c+temp_r*temp_r);
    float temp_x = 0.1118f*temp_xie-7.651;
    _POINT_ tarP;
    tarP.x = -temp_x-2.5;
    tarP.y = 0;
    tarP.z = 6;
    tarP.q = temp_q;
    qDebug() << "chase:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;

    this->ui->lineEdit_targetX->setText(QString::number(-temp_x-2.5 ,'f',2));
    this->ui->lineEdit_targetY->setText("0");
    this->ui->lineEdit_targetZ->setText("6");
    this->ui->lineEdit_targetQ->setText(QString::number(temp_q ,'f',2));
    coordinate(&tarP);
}

void MainWindow::jia_throw()
{
    dilver_check();//传送带停止
    delay(1000);
    serial_port_t->write("M3 S70\r\n");
    chase();//运动到上方
    delay(1000);

    QString str_x=ui->lineEdit_targetX->text();
    float x = str_x.toFloat();
    QString str_y=ui->lineEdit_targetY->text();
    float y = str_y.toFloat();
    QString str_z=ui->lineEdit_targetZ->text();
    float z = str_z.toFloat();
    QString str_q=ui->lineEdit_targetQ->text();
    float q = str_q.toFloat();
    _POINT_ tarP;
    tarP.x = x;
    tarP.y = y;
    tarP.z = 4;
    tarP.q = q;
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);//运动到物体表面
    delay(2000);
    serial_port_t->write("M3 S53\r\n");
    tarP.x=tarP.x/2;
    tarP.z=6;
    coordinate(&tarP);//抽回机械臂
    delay(1500);
    tarP.x=-8;
    tarP.z=6;
    tarP.q=-13;
    coordinate(&tarP);//丢掉物体
    delay(2000);
    serial_port_t->write("M3 S70\r\n");
    coordinate(&P_org);//回到原点
    delay(2000);
    dilver_check();//传送带运动
}
void MainWindow::pick_throw()
{
    dilver_check();//传送带停止
    delay(1000);
    chase();//运动到上方
    delay(1000);
    qiB_check();//打开气泵
    delay(500);


    QString str_x=ui->lineEdit_targetX->text();
    float x = str_x.toFloat();
    QString str_y=ui->lineEdit_targetY->text();
    float y = str_y.toFloat();
    QString str_z=ui->lineEdit_targetZ->text();
    float z = str_z.toFloat();
    QString str_q=ui->lineEdit_targetQ->text();
    float q = str_q.toFloat();
    _POINT_ tarP;
    tarP.x = x;
    tarP.y = y;
    tarP.z = 0.5;
    tarP.q = q;
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);//运动到物体表面
    delay(2000);
    tarP.x=tarP.x/2;
    tarP.z=6;
    coordinate(&tarP);//抽回机械臂
    delay(1500);
    tarP.x=-8;
    tarP.z=10;
    tarP.q=-13;
    coordinate(&tarP);//丢掉物体
    delay(2000);
    elec_check();//打开电磁阀
    delay(200);
    elec_check();//关掉电磁阀
    delay(200);
    qiB_check();//关掉气泵
    delay(200);
    coordinate(&P_org);//回到原点
    delay(2000);
    dilver_check();//传送带运动
}
void MainWindow::on_pushButton_chase_clicked()
{
//    QFile file("P:\\Item\\QT\\ARM_Controller\\temp.txt");
//    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        qDebug()<<"Can't open the file!"<<endl;
//    }
//    QByteArray line;
//    QString temp_str;
//    //dilver_check();//传送带启动
//    //delay(800);
//    while(true){
//        line = file.readLine();
//        temp_str= QString(line);
//        qDebug()<< temp_str.length()<<endl;
//        if(temp_str.length()>=50&&temp_str.length()<=60){
//            QStringList xyz = temp_str.split(".), tensor(");
//            int indexOfEth0= xyz.at(0).indexOf("or(");
//            int leftup_data_x = xyz.at(0).mid(indexOfEth0+3).toDouble();
//            int leftup_data_y = xyz.at(1).toDouble();
//            int rightdown_data_x = xyz.at(2).toDouble();
//            int indexOfEth1= xyz.at(3).indexOf("or(");
//            int indexOfEth2= xyz.at(3).indexOf(".)]");
//            int rightdown_data_y = xyz.at(3).mid(indexOfEth1,indexOfEth2-indexOfEth1).toDouble();
//            int temp_area=(rightdown_data_x-leftup_data_x)*(rightdown_data_y-leftup_data_y);
//            if(leftup_data_y>20&&temp_area<33000&&temp_area>15000)
//                break;
//        }
//    }
    chase();
    //pick_throw();
    //jia_throw();
}
void MainWindow::on_pushButton_OrgPoint_clicked()
{
    this->ui->lineEdit_targetX->setText(QString::number(P_org.x ,'f',2));
    this->ui->lineEdit_targetY->setText(QString::number(P_org.y ,'f',2));
    this->ui->lineEdit_targetZ->setText(QString::number(P_org.z ,'f',2));
    this->ui->lineEdit_targetQ->setText(QString::number(P_org.q ,'f',2));
    coordinate(&P_org);
}

void MainWindow::on_checkBox_up_stateChanged(int arg1)
{
//    .x = -3.5,
//    .y = 0,
//    .z = 8,
//    .q = 0
    _POINT_ tarP;
    tarP.x = P_now.x;
    tarP.y = P_now.y;
    tarP.z = P_now.z+1;
    tarP.q = EEG_q;
    tarP.z = _ilimit(tarP.z,1,16);
    tarP.x = _ilimit(tarP.x,-19,-3.5);
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);
}

void MainWindow::on_checkBox_down_stateChanged(int arg1)
{
    _POINT_ tarP;
    tarP.x = P_now.x;
    tarP.y = P_now.y;
    tarP.z = P_now.z-1;
    tarP.q = EEG_q;
    tarP.z = _ilimit(tarP.z,1,16);
    tarP.x = _ilimit(tarP.x,-23,-3.5);
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);
}

void MainWindow::on_checkBox_left_stateChanged(int arg1)
{
    _POINT_ tarP;
    tarP.x = P_now.x;
    tarP.y = P_now.y;
    tarP.z = P_now.z;
    tarP.q = EEG_q;
    tarP.z = _ilimit(tarP.z,1,16);
    tarP.x = _ilimit(tarP.x,-19,-3.5);
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);
}

void MainWindow::on_checkBox_right_stateChanged(int arg1)
{
    _POINT_ tarP;
    tarP.x = P_now.x;
    tarP.y = P_now.y;
    tarP.z = P_now.z;
    tarP.q = EEG_q;
    tarP.z = _ilimit(tarP.z,1,16);
    tarP.x = _ilimit(tarP.x,-19,-3.5);
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);
}

void MainWindow::on_checkBox_rotleft_stateChanged(int arg1)
{
    EEG_q = EEG_q-2;
    _POINT_ tarP;
    tarP.x = P_now.x;
    tarP.y = P_now.y;
    tarP.z = P_now.z;
    tarP.q = EEG_q;
    tarP.z = _ilimit(tarP.z,1,16);
    tarP.x = _ilimit(tarP.x,-19,-3.5);
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);
}

void MainWindow::on_checkBox_rotright_stateChanged(int arg1)
{
    EEG_q = EEG_q+2;
    _POINT_ tarP;
    tarP.x = P_now.x;
    tarP.y = P_now.y;
    tarP.z = P_now.z;
    tarP.q = EEG_q;
    tarP.z = _ilimit(tarP.z,1,16);
    tarP.x = _ilimit(tarP.x,-19,-3.5);
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);
}

void MainWindow::on_checkBox_forward_stateChanged(int arg1)
{
    _POINT_ tarP;
    tarP.x = P_now.x-1;
    tarP.y = P_now.y;
    tarP.z = P_now.z;
    tarP.q = EEG_q;
    tarP.z = _ilimit(tarP.z,1,16);
    tarP.x = _ilimit(tarP.x,-19,-3.5);
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);
}

void MainWindow::on_checkBox_backward_stateChanged(int arg1)
{
    _POINT_ tarP;
    tarP.x = P_now.x+1;
    tarP.y = P_now.y;
    tarP.z = P_now.z;
    tarP.q = EEG_q;
    tarP.z = _ilimit(tarP.z,5,13);
    tarP.x = _ilimit(tarP.x,-15,-3.5);
    qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
    coordinate(&tarP);
}
void MainWindow::Timer_func()
{
    // 读取内容 放入到textEdt中
    QFile file(path);  // 参数就是选取文件的路径
    // 设置打开方式
    file.open(QIODevice::ReadOnly);

    QByteArray array = file.readAll();
    now_txt = QString(array);
    if (last_txt != now_txt){
        send_txtdata(now_txt);
        qDebug()<<array<<endl;
    }
    last_txt = now_txt;
    //qDebug()<<array<<endl;

}
void MainWindow::send_txtdata(QString data){
    int tmp = data.toInt();

    switch(tmp){
        _POINT_ tarP;
        case 1  :
            tarP.x = P_now.x;
            tarP.y = P_now.y;
            tarP.z = P_now.z+6.5;
            tarP.q = EEG_q;
            tarP.z = _ilimit(tarP.z,1,16);
            tarP.x = _ilimit(tarP.x,-19,-3.5);
            qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
            coordinate(&tarP);
            break; /* 可选的 */
        case 4  :
            tarP.x = P_now.x;
            tarP.y = P_now.y;
            tarP.z = P_now.z-4.5;
            tarP.q = EEG_q;
            tarP.z = _ilimit(tarP.z,1,16);
            tarP.x = _ilimit(tarP.x,-19,-3.5);
            qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
            coordinate(&tarP);
            break; /* 可选的 */
        case 3  :
            EEG_q = EEG_q+2;
            tarP.x = P_now.x;
            tarP.y = P_now.y;
            tarP.z = P_now.z;
            tarP.q = EEG_q;
            tarP.z = _ilimit(tarP.z,1,16);
            tarP.x = _ilimit(tarP.x,-19,-3.5);
            qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
            coordinate(&tarP);
            break; /* 可选的 */
        case 2  :
            EEG_q = EEG_q-2;
            tarP.x = P_now.x;
            tarP.y = P_now.y;
            tarP.z = P_now.z;
            tarP.q = EEG_q;
            tarP.z = _ilimit(tarP.z,1,16);
            tarP.x = _ilimit(tarP.x,-19,-3.5);
            qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
            coordinate(&tarP);
            break; /* 可选的 */
        case 5  :
            tarP.x = P_now.x-7;
            tarP.y = P_now.y;
            tarP.z = P_now.z;
            tarP.q = EEG_q;
            tarP.z = _ilimit(tarP.z,1,16);
            tarP.x = _ilimit(tarP.x,-19,-3.5);
            qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
            coordinate(&tarP);
            break; /* 可选的 */
        case 6  :
            tarP.x = P_now.x+7;
            tarP.y = P_now.y;
            tarP.z = P_now.z;
            tarP.q = EEG_q;
            tarP.z = _ilimit(tarP.z,1,16);
            tarP.x = _ilimit(tarP.x,-19,-3.5);
            qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
            coordinate(&tarP);
            break; /* 可选的 */
        case 7  :
            EEG_q = EEG_q-5;
            tarP.x = P_now.x;
            tarP.y = P_now.y;
            tarP.z = P_now.z;
            tarP.q = EEG_q;
            tarP.z = _ilimit(tarP.z,1,16);
            tarP.x = _ilimit(tarP.x,-19,-3.5);
            qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
            coordinate(&tarP);
            break; /* 可选的 */
        case 8  :
            EEG_q = EEG_q+5;
            tarP.x = P_now.x;
            tarP.y = P_now.y;
            tarP.z = P_now.z;
            tarP.q = EEG_q;
            tarP.z = _ilimit(tarP.z,1,16);
            tarP.x = _ilimit(tarP.x,-19,-3.5);
            qDebug() << "Dian:"<< tarP.x <<" "<<tarP.y<<" "<<tarP.z<< endl;
            coordinate(&tarP);
            break; /* 可选的 */

        /* 您可以有任意数量的 case 语句 */
        default : /* 可选的 */
           break;
    }
}

