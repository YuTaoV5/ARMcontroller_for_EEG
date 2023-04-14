#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "hidapi.h"
#include <QMainWindow>
#include <QSerialPort>      //提供访问串口的功能
#include <QSerialPortInfo>  //提供系统中存在的串口的信息
#include <QDebug>
#include <QMessageBox>
#include <QTimerEvent>
#include <QString>
#include <QTimer>
#include <qcustomplot.h>
#include <QMetaEnum>
#include <QScreen>
//#include "tcpclient.h"
#include <QImage>
#include <QPixmap>
#include <QtMath>
#include <QtNetwork/QTcpSocket>
#include <QDataStream>
#include <QByteArray>
#include <QSqlTableModel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <ActiveQt/QAxWidget>
#include <QTextStream>
#include <QSqlError>
#include "connect.h"

namespace Ui {
class MainWindow;
}

typedef struct
{
    float x;
    float y;
    float z;
    float q;
}_POINT_;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    int gamedata[8];
    int ReturnFlag;
    int Mytime;

    _POINT_ P_now;
    _POINT_ P_org;
    _POINT_ P_CatchUp;
    _POINT_ P_CatchDown;
    _POINT_ P_CatchSky;

    QVector<double> x,y,z,time_count;
    int zitai_count;
    int signal_ele;
    int signal_qiB;
    int signal_deliver;
    float x_data;//,kal_x;
    float y_data;//,kal_y;
    float z_data;//,kal_z;
    float Temper;
    float Arm_z;
    float Arm_y;
    float Arm_x;
    float Arm_u;
    float EEG_q;
    int flag_step1=0;
    int last_step=0;
    void showdata();
    QString path;
    void coordinate(_POINT_* tar);//从(x1,y1)移动到(x2,y2)
    //void coordinate_neo(_POINT_* tar);//从(x1,y1)移动到(x2,y2)
    void move(char x, char y);
    void hannuo(int n,char one ,char two,char three);
    void recognize(int num);
    void chase();
    QByteArray ba,sendarray;
    void qiB_check();
    void elec_check();
    void dilver_check();
    void pick_throw();
    void jia_throw();
    QString demoName;
    hid_device *handle;
    QSerialPort myserial;
    QSerialPort *serial_port_t;
    bool m_bIsOpenPort;         //是否已打开串口
    int controlflag;
    bool hex_send;
    bool hex_receive;
    int last_game[8];
    QTcpSocket * socket;
    QString now_txt;
    QString last_txt;
    QLineEdit *lineUrl;
    QAxWidget* webWidget;

    QSqlTableModel *mymodel;

//    TcpClient *Client;
//    QString m_HttpBoundary;
//    QString m_JpgBuff;
//    bool m_bflagHttpStream;
//    bool stream_head_start;
//    bool analysis_imgdata;
//    bool m_bflagBodyEnd;
//    int m_length;
    ~MainWindow();
public slots:
    void send_txtdata(QString data);
    void read_gameController();
    void show_wave();
    void timeEvent();
    void send_zitai_data();
    void delay(int msec);
    void Water_timeEvent();
    void Timer_func();
private:
    Ui::MainWindow *ui;
    int res;    
    int finish_flag;

    void baud_activated(const QString &arg1);

    void jiaoyan_activated(int arg1);

    void date_activated(const QString &arg1);

    void stop_activated(const QString &arg1);

//    void sendHttpRequest();

//    void dataReceived();

//    void image_signal(QImage img);
private slots:   
    void loadNavigate();

    void clear_data();

    void on_open_clicked();

    void on_clear_clicked();

    void on_send_clicked();

    void read_date();

    void on_checkBox_send_stateChanged();

    void on_checkBox_receive_stateChanged();

    void read_COM();

    void setupQuadraticDemo(QCustomPlot *customPlot);

    void on_control_activated();

    void on_pushButton_up_clicked();

    void on_pushButton_right_clicked();

    void on_pushButton_left_clicked();

    void on_pushButton_down_clicked();

    void on_pushButton_return_clicked();

    void on_pushButton_clear_clicked();
    void on_pushButton_Rect_clicked();
    void on_pushButton_ll_clicked();
    void on_pushButton_rr_clicked();

    void on_pushButton_database_clicked();
    void on_pushButton_add_clicked();
    void on_pushButton_del_clicked();
    void on_pushButton_again_clicked();

    void connected(); //已经连接
    void readyread(); //准备读取


    void on_PushButton_hannuo_clicked();
    void on_pushButton_clicked();
    void on_pushButton_qiB_clicked();
    void on_pushButton_elec_clicked();
    void on_pushButton_dilver_clicked();
    void on_pushButton_chase_clicked();
    void on_pushButton_OrgPoint_clicked();
    void on_checkBox_up_stateChanged(int arg1);
    void on_checkBox_down_stateChanged(int arg1);
    void on_checkBox_left_stateChanged(int arg1);
    void on_checkBox_right_stateChanged(int arg1);
    void on_checkBox_rotleft_stateChanged(int arg1);
    void on_checkBox_rotright_stateChanged(int arg1);
    void on_checkBox_forward_stateChanged(int arg1);
    void on_checkBox_backward_stateChanged(int arg1);
};

#endif // MAINWINDOW_H
