#include "mainwindow.h"
#include <QApplication>

int flag = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.gst = new Gst();
    w.gst->qtVars = new InitQtVariables();
    w.gst->qtVars->encoderType = new char[20];
    w.gst->fileLocation = new char[100];
    qDebug() << " main filelocation %p" <<w.gst->fileLocation;
    w.gst->qtVars->containerFormat = new char[20];
    w.gst->command = new char[50];

    strcpy( w.gst->fileLocation, "D:\\test.avi");

    w.gst->qtVars->topLeftX = 0;
    w.gst->qtVars->topLeftY = 0;
    w.gst->qtVars->width    = 0;
    w.gst->qtVars->height   = 0;
    w.gst->t_SetTime        = 0;

    gst_init (&argc,&argv);
    w.show();

    return a.exec();
}
