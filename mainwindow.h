#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <gst/gst.h>
#include <glib.h>
#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <iostream>



//default values for co-ordinates and frame rate and co-ordinates
#define MAX_TOP_LEFT_X                1023
#define MAX_TOP_LEFT_Y                765
#define MAX_WIDTH                     1024
#define MAX_HEIGHT                    768
#define MAX_FPS                       60
#define MIN_TOP_LEFT_X                1
#define MIN_TOP_LEFT_Y                1
#define MIN_WIDTH                     1
#define MIN_HEIGHT                    1
#define MIN_FPS                       5
#define DEFAULT_TOP_LEFT_X            1
#define DEFAULT_TOP_LEFT_Y            1000
#define DEFAULT_WIDTH                 640
#define DEFAULT_HEIGHT                480
#define DEFAULT_FPS                   20

typedef struct
{
    int   topLeftX;
    int   topLeftY;
    int   height;
    int   width;
    int   fps;
   char* encoderType;
    char* containerFormat;

}InitQtVariables;

typedef struct
{
    GstElement*       pipeline;
    GstElement*       source;
    GstElement*       filter;
    GstElement*       ffmpegcolorspace;
    GstElement*       accel;
    GstElement*       encoder;
    GstElement*       queue;
    GstElement*       muxer;
    GstElement*       sink;
    GMainLoop*        loop;
    GstCaps*          video_caps;
    GstBus*           bus;
    guint             bus_watch_id;
    GObject           *startButton;
    GObject           *stopButton;
    GObject           *TimeDisplay;
    GObject           *TimeRemained;
    char              *fileLocation;
    time_t            t_start, t_end;
    time_t            t_SetTime,t_Rawtime;
    InitQtVariables*  qtVars;
    char*             command;

    QTimer* timer;

}Gst;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     Gst* gst = NULL;
     int flag = 1;

private slots:

    void allocateMemory();
    void on_ComboBoxContainerFormat_currentIndexChanged();

    void on_ComboBoxEncoderFormat_currentIndexChanged();

    void on_ComboBoxFPS_currentTextChanged();

    void on_ButtonFileSelect_clicked();

    void on_ButtonStart_clicked();

    void on_ButtonStop_clicked();

    bool fileExists(const char* file);

    int start_pipeline( gpointer   data );

    int stop_pipeline( gpointer   data );

    int bus_watcher( Gst* gst );

    int pipeline_make( Gst* gst );

    int create_elements( Gst* gst, char* location );

    int state_handler( Gst* gst, GstState state );

    void showTime();

private:
    Ui::MainWindow *ui;
    GstBusFunc *GstBusFuncPtr;
};

#endif // MAINWINDOW_H
