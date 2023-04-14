#include "mainwindow.h"
#include <QApplication>
#include <QTimer>
#include "connect.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString qss;
    QFile qssFile("P:/Item/QT/ARM_Controller/main.qss");
    qssFile.open(QFile::ReadOnly);

    if(qssFile.isOpen())
    {
        qss = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(qss);
        qssFile.close();
    }
    //a.setStyle(QStyleFactory::create("vis"));
    MainWindow w;
    w.show();    
    return a.exec();
}
