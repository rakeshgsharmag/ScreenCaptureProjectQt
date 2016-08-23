#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //QPixmap pix(":\Logo_Image.png");
    //ui->label->setPixmap(pix);
    ui->ComboBoxFPS->addItem("5");
    ui->ComboBoxFPS->addItem("10");
    ui->ComboBoxFPS->addItem("15");
    ui->ComboBoxFPS->addItem("20");
    ui->ComboBoxFPS->addItem("25");
    ui->ComboBoxFPS->addItem("30");
    ui->ComboBoxFPS->addItem("35");

    ui->ComboBoxContainerFormat->addItem("AVI");
    ui->ComboBoxContainerFormat->addItem("MP4");

    ui->ComboBoxEncoderFormat->addItem("H264");
    ui->ComboBoxEncoderFormat->addItem("MPEG-4");

    /*QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start();*/
}

MainWindow::~MainWindow()
{
    if(gst)
    {
        if(gst->qtVars)
        {
            if( gst->qtVars->containerFormat)
                delete[] gst->qtVars->containerFormat;
            if( gst->qtVars->encoderType )
                delete[] gst->qtVars->encoderType;
            delete[] gst->qtVars;
        }
   delete[] gst;
    }
    delete ui;
}

void MainWindow::allocateMemory()
{
    if(!gst)
    {
        qDebug() <<"Allocating inside combobox";
        gst = new Gst();
        gst->qtVars = new InitQtVariables();
        gst->qtVars->encoderType = new char[20];
        gst->fileLocation = new char[100];
        gst->qtVars->containerFormat = new char[20];
    }
}

void MainWindow::on_ComboBoxContainerFormat_currentIndexChanged()
{
    qDebug()<<ui->ComboBoxContainerFormat->currentText();

    allocateMemory();

    QByteArray inBytes;
    const char *cStrData;

    inBytes = ui->ComboBoxContainerFormat->currentText().toUtf8();
    cStrData = inBytes.constData();

    qDebug()<<"str" <<cStrData;
    strcpy( gst->qtVars->containerFormat, cStrData );
    //qDebug() << "filelocation %s" << gst->qtVars->containerFormat;
}

void MainWindow::on_ComboBoxEncoderFormat_currentIndexChanged()
{
    allocateMemory();

    QByteArray inBytes;
    const char *cStrData;

    inBytes = ui->ComboBoxEncoderFormat->currentText().toUtf8();
    cStrData = inBytes.constData();

    qDebug()<<"str" <<cStrData;
    strcpy( gst->qtVars->encoderType, cStrData );
    qDebug() << "Encoder Type : " << gst->qtVars->encoderType;
}

void MainWindow::on_ComboBoxFPS_currentTextChanged()
{
    allocateMemory();
    QByteArray inBytes;
    const char *cStrData;

    inBytes = ui->ComboBoxFPS->currentText().toUtf8();
    cStrData = inBytes.constData();

    qDebug()<<"str" <<cStrData;

    gst->qtVars->fps = atoi(cStrData);
    qDebug() <<"Fps = "<<gst->qtVars->fps;

}

void MainWindow::on_ButtonFileSelect_clicked()
{
    allocateMemory();
    QString filename = QFileDialog::getSaveFileName(this , tr("open file"),"C://",tr("Videos (*.avi *.mp4 )"));

    QByteArray inBytes;
    const char *cStrData;

    inBytes = filename.toUtf8();
    cStrData = inBytes.constData();

    qDebug()<<"str" <<cStrData;
    strcpy(gst->fileLocation, cStrData);
    ui->lineEdit_5->setText(cStrData);
}

void  MainWindow::showTime()
{
    long long int hour = (( ui->dateTimeEdit->time().hour() )* 3600 * 1000 );
    long long int min  = ((ui->dateTimeEdit->time().minute() )*60*1000);
    long long int sec  = ((ui->dateTimeEdit->time().second())*1000);

    //qDebug() << "hour : " <<hour;
    //qDebug() << "min : " << min;
    //qDebug() << "secs : " << sec;
    long long int time_text;
    time_text = ( hour + min + sec )- ui->dateTimeEdit->time().elapsed();
    //qDebug()<<"Time Text :" <<time_text;
    if( time_text == 0 )
        flag = 0;
}

void MainWindow::on_ButtonStart_clicked()
{
    allocateMemory();
    //start pipeline here
    qDebug()<<"start pressed";
    ui->ButtonStart->setDisabled(true);
    ui->ButtonStop->setEnabled(true);

    QByteArray inBytes;
    const char *cStrData;
    inBytes = ui->LineEditTopX->text().toUtf8();
    cStrData = inBytes.constData();

    qDebug()<<"Your X coordinate : " <<cStrData;
    gst->qtVars->topLeftX = atoi(cStrData);
    qDebug() << "TopLeftX coordinate : "<<gst->qtVars->topLeftX;

    inBytes = ui->lineEdit_3->text().toUtf8();
    cStrData = inBytes.constData();
    qDebug()<<"Your Y coordinate : " <<cStrData;
    gst->qtVars->topLeftY = atoi(cStrData);
    qDebug() << "TopLeftY coordinate : "<<gst->qtVars->topLeftY;

    inBytes = ui->lineEdit_2->text().toUtf8();
    cStrData = inBytes.constData();
    qDebug()<<"Heigth : " <<cStrData;
    gst->qtVars->height = atoi(cStrData);
    qDebug() << "Heigth : "<<gst->qtVars->height;

    inBytes = ui->lineEdit_4->text().toUtf8();
    cStrData = inBytes.constData();
    qDebug()<<"Width: " <<cStrData;
    gst->qtVars->width = atoi(cStrData);
    qDebug() << "Width : "<<gst->qtVars->width;

    gst->timer = new QTimer(this);
    connect(gst->timer, SIGNAL(timeout()), this, SLOT(showTime()));
    gst->timer->start();
    if( flag == 0 )
    {
        gst->timer->stop();
        stop_pipeline(gst);
    }
    qDebug()<<"calling start pipeline";
    start_pipeline( gst );
}

void MainWindow::on_ButtonStop_clicked()
{
    //stop pipeline here
    qDebug()<<"stop pressed";
    ui->ButtonStop->setDisabled(true);
    ui->ButtonStart->setEnabled(true);

    stop_pipeline( gst );
    gst->timer->stop();
    int temp = ui->dateTimeEdit->time().elapsed();
    qDebug()<<"Elapsed Time : "<<temp;
}

bool MainWindow::fileExists(const char* file)
{
    struct stat buf;
    return (stat(file, &buf) == 0);
}

int MainWindow::state_handler( Gst* gst, GstState state )
{
    GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
    if( state == (GstStateChangeReturn) GST_STATE_NULL )
    {
        ret = gst_element_set_state( gst->source, GST_STATE_PAUSED );
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            qDebug()<<"failed to change state ret = " <<ret;
            return -1;
        }
        ret = gst_element_set_state ( gst->source, GST_STATE_READY);
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            qDebug()<<"failed to change state ret = "<< ret;
            return -1;
        }
        ret = gst_element_set_state( gst->source, GST_STATE_NULL );
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            qDebug()<<"failed to change state ret = "<<ret;
            return -1;
        }

        ret = gst_element_set_state( gst->pipeline, GST_STATE_PAUSED );
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            qDebug()<<"failed to change state ret = "<<ret;
            return -1;
        }
        ret = gst_element_set_state( gst->pipeline, GST_STATE_READY);
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            qDebug()<<"failed to change state ret = "<<ret;
            return -1;
        }
        ret = gst_element_set_state( gst->pipeline, GST_STATE_NULL );
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            qDebug()<<"failed to change state ret = "<<ret;
            return -1;
        }
    }
    else if (state == (GstStateChangeReturn) GST_STATE_PLAYING)
    {

        ret = gst_element_set_state ( gst->pipeline, GST_STATE_READY);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            qDebug()<<"failed to change state ret = "<<ret;
            return -1;
        }
        ret = gst_element_set_state ( gst->pipeline, GST_STATE_PAUSED);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            qDebug()<<"failed to change state ret = "<<ret;
            return -1;
        }
        ret = gst_element_set_state ( gst->pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            qDebug()<<"failed to change state ret = "<<ret;
            return -1;
        }
    }
    return 0;
}

static gboolean my_bus_callback( GstBus* bus, GstMessage *message, gpointer data )
{
    switch( GST_MESSAGE_TYPE (message) )
    {
        case GST_MESSAGE_ERROR:
        {
            qDebug()<<"\nGot message"<<GST_MESSAGE_TYPE_NAME (message);
            GError *err;
            gchar *debug;
            gst_message_parse_error( message, &err, &debug );
            qDebug()<<"\nError: "<< err->message;
            g_error_free (err);
            g_free (debug);
            break;
        }
        case GST_MESSAGE_EOS:
        {
            qDebug()<<"\nGot message"<<GST_MESSAGE_TYPE_NAME (message);
            /* end-of-stream */
            break;
        }
        default:
            /* unhandled message */
            break;
    }
    /* we want to be notified again the next time there is a message
    * on the bus, so returning TRUE (FALSE means we want to stop watching
    * for messages on the bus and our callback should not be called again)
    */
    return TRUE;
}

int MainWindow::create_elements( Gst* gst, char* location )
{
    GstElement *encoder;
    GstElement *muxer;

    if ( location == NULL )
    {
        qDebug()<<"\nAllocating memory to location";
        location = ( char*)malloc(100);
        if( location == NULL )
        {
            GST_ERROR( "Malloc Failed\n" );
            return -1;
        }
    }

    /* Create gstreamer elements */
    if (!gst->pipeline)
    {
        qDebug()<<"\nCreating pipeline";
        gst->pipeline = gst_pipeline_new( "pipeline" );
    }

    if (!gst->source)
    {
        qDebug()<<"\nCreating source";
        gst->source = gst_element_factory_make( "gdiscreencapsrc",    "videotestsrc" );
    }

    if (!gst->filter)
    {
        qDebug()<<"\nCreating filter";
        gst->filter = gst_element_factory_make( "capsfilter", "filter" );
        /* Set up elements */
        g_object_set( G_OBJECT ( gst->filter ), "caps", gst->video_caps, NULL );
    }

    if (!gst->ffmpegcolorspace)
    {
        qDebug()<<"\nCreating ffmpegcolorspace";
        gst->ffmpegcolorspace = gst_element_factory_make( "ffmpegcolorspace", "ffmpegcolorspace" );
    }

    /* Create Encoder */
    encoder = gst_bin_get_by_name (GST_BIN (gst->pipeline), "encoder");
    if (encoder)
    {
        qDebug()<<"\nRemoving encoder";
        gst_bin_remove(GST_BIN (gst->pipeline), encoder);
        gst_object_unref (GST_OBJECT (gst->encoder));
    }

    qDebug()<<"\nCreating encoder";
    qDebug()<<"\nEncoder type in create_elements "<< gst->qtVars->encoderType;
    if( strcmp( gst->qtVars->encoderType, "Encoder_H264" ) == 0 )
    {
        gst->encoder = gst_element_factory_make( "x264enc",    "encoder" );
    }
    else
    {
        qDebug()<<"creating x264enc";
        gst->encoder = gst_element_factory_make( "x264enc",    "encoder" );
    }

    if (encoder)
    {
        qDebug()<<"\nAdding encoder";
        gst_bin_add(GST_BIN (gst->pipeline), gst->encoder);
    }

    /* Create Muxer */
    muxer = gst_bin_get_by_name (GST_BIN (gst->pipeline), "mux");
    if (muxer)
    {
        qDebug()<<"\nRemoving muxer";
        gst_bin_remove(GST_BIN (gst->pipeline), muxer);
        gst_object_unref (GST_OBJECT (gst->muxer));
    }

    qDebug()<<"\nCreating muxer";
    if( strcmp( gst->qtVars->containerFormat, "Container_AVI" ) == 0 )
    {
        gst->muxer = gst_element_factory_make( "avimux", "mux" );
    }
    else if( strcmp( gst->qtVars->containerFormat, "Container_mp4" ) == 0 )
    {
        qDebug()<<"\n creating mpeg4mux";
        gst->muxer = gst_element_factory_make( "mp4mux", "mux" );
    }
    else
    {
        gst->muxer = gst_element_factory_make( "avimux", "mux" );
    }

    qDebug()<<"muxer type in create_elements "<< gst_element_get_name(gst->muxer);

    if (encoder)
    {
        qDebug()<<"\nAdding Muxer";
        gst_bin_add(GST_BIN (gst->pipeline), gst->muxer);
    }

    if (!gst->sink)
    {
        qDebug()<<"\nCreating sink";
        gst->sink = gst_element_factory_make( "filesink", "filesink" );
        qDebug()<<"sink type in create_elements "<< gst_element_get_name(gst->sink);

        /* we set the input filename to the source element */
        g_object_set( G_OBJECT( gst->sink ), "location", gst->fileLocation, NULL );
        qDebug()<<"\nLocation = "<<gst->fileLocation;
    }

    if (!gst->qtVars->fps)
        gst->qtVars->fps = DEFAULT_FPS;

    if (!gst->video_caps)
    {
        qDebug()<<"\nCreating video_caps";
        /* Video caps */
        gst->video_caps = gst_caps_new_simple( "video/x-raw-rgb", "framerate", GST_TYPE_FRACTION, gst->qtVars->fps, 1, NULL );
        gst_caps_unref( gst->video_caps );
    }

    if( !gst->pipeline || !gst->source || !gst->filter || !gst->encoder || !gst->muxer || !gst->sink )
    {
        GST_ERROR ( "One element could not be created. Exiting.\n" );
        return -1;
    }

    if (gst->qtVars->topLeftX)
        g_object_set( G_OBJECT( gst->source ), "x", gst->qtVars->topLeftX, NULL );
    if (gst->qtVars->topLeftY)
        g_object_set( G_OBJECT( gst->source ), "y", gst->qtVars->topLeftY, NULL );
    if (gst->qtVars->width)
        g_object_set( G_OBJECT( gst->source ), "width", gst->qtVars->width, NULL );
    if (gst->qtVars->height)
        g_object_set( G_OBJECT( gst->source ), "height", gst->qtVars->height, NULL );

    qDebug()<<"size = "<<gst->qtVars->topLeftX << "X" << gst->qtVars->topLeftY <<
                    "WH = "<< gst->qtVars->width << "X" <<gst->qtVars->height;

    return 0;
}

int MainWindow::pipeline_make( Gst* gst )
{
    gboolean ret = false;
    if (!gst_bin_get_by_name (GST_BIN (gst->pipeline), "filesink"))
    {
        /* Add all elements into the pipeline */
        gst_bin_add_many( GST_BIN ( gst->pipeline), gst->source, gst->filter, gst->ffmpegcolorspace, gst->encoder,
                         gst->muxer, gst->sink, NULL);
    }

    /* Link the elements together */
    ret = gst_element_link_many( gst->source, gst->filter, gst->ffmpegcolorspace,  gst->encoder, gst->muxer, gst->sink, NULL );
    qDebug()<<" linking elements return type = "<<ret;
    if (ret == false)
    {
        ret = gst_element_link( gst->encoder, gst->muxer );
        qDebug()<<" linking elements return type = "<<ret;
        ret = gst_element_link( gst->muxer, gst->sink );
        qDebug()<<" linking elements return type = "<<ret;
        ret = gst_element_link( gst->ffmpegcolorspace, gst->encoder);
        qDebug()<<" linking elements return type = "<<ret;

    }
    return 0;
}

int MainWindow::bus_watcher( Gst* gst )
{
    /* Add a message handler */
    gst->bus = gst_pipeline_get_bus( GST_PIPELINE( gst->pipeline) );
    gst->bus_watch_id = gst_bus_add_watch( gst->bus, (GstBusFunc)my_bus_callback, gst );
    gst_object_unref( gst->bus );
    return 0;
}

/* This is a callback function. */
int MainWindow::stop_pipeline( gpointer   data )
{
    Gst* gst = ( Gst* ) data;
    double difft = 0;

    g_print ("Stop Recording ...");

    /* take the current time end time */
    time (&gst->t_end);

    /* calculate time spent */
    difft = difftime (gst->t_end,gst->t_start);
    gst->t_end=0;
    g_print ("Total time = %f seconds\n", difft);

    if( state_handler( gst, GST_STATE_NULL) !=0 )
        return -1;
    return 0;
}

int MainWindow::start_pipeline( gpointer   data )
{

    Gst* gst = ( Gst* ) data;
    char loc[] = "D:\\test.avi";

    /* Initialize elements */
    if( create_elements( gst, loc) != 0 )
        return -1;

    /* Add function to watch bus */
    if( bus_watcher( gst ) != 0 )
        return -1;

    /* Add elements to pipeline, and link them */
    if( pipeline_make( gst ) != 0 )
        return -1;

    /* Set the pipeline to "playing" state*/
    if( state_handler( gst, GST_STATE_PLAYING) !=0 )
        return -1;

    /* take the current time start time */
    time (&gst->t_start);

    /* To obtain .dot files, set the GST_DEBUG_DUMP_DOT_DIR environment
    variable to point to the folder where you want the files to be placed. */

    GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS (GST_BIN(gst->pipeline),
    GST_DEBUG_GRAPH_SHOW_ALL, "gstcapture-1.0-playing");

    g_print ("Start Recording ...");

    return 0;
}

