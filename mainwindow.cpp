#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

    gst->loop = g_main_loop_new (NULL, FALSE);
    gst->cmd = "";
}

MainWindow::~MainWindow()
{
    GST_DEBUG ("\nIn Destructor\n");
    if(gst)
    {
       if(gst->qtVars)
       {
           if( gst->qtVars->containerFormat)
             delete[] gst->qtVars->containerFormat;
           if( gst->qtVars->encoderType )
               delete[] gst->qtVars->encoderType;
           delete gst->qtVars;
       }
       delete gst;
    }
    delete ui;
}

void MainWindow::allocateMemory()
{
    if(!gst)
    {
        GST_DEBUG ("\nAllocating inside combobox\n");
        gst = new Gst();
        gst->qtVars = new InitQtVariables();
        gst->qtVars->encoderType = new char[20];
        gst->fileLocation = new char[100];
        gst->qtVars->containerFormat = new char[20];
        gst->command = new char[50];
    }
}

void MainWindow::on_ComboBoxContainerFormat_currentIndexChanged()
{
    allocateMemory();

    QByteArray inBytes;
    const char *cStrData;

    inBytes = ui->ComboBoxContainerFormat->currentText().toUtf8();
    cStrData = inBytes.constData();

    GST_DEBUG ("\nstring %s",cStrData);
    strcpy( gst->qtVars->containerFormat, cStrData );
    GST_DEBUG ("\nfilelocation %s",gst->qtVars->containerFormat);
}


void MainWindow::on_ComboBoxEncoderFormat_currentIndexChanged()
{

    allocateMemory();

    QByteArray inBytes;
    const char *cStrData;

    inBytes = ui->ComboBoxEncoderFormat->currentText().toUtf8();
    cStrData = inBytes.constData();

    GST_DEBUG ("\nstr %s",cStrData);
    strcpy( gst->qtVars->encoderType, cStrData );
    GST_DEBUG ("\nEncoder Type : %s",gst->qtVars->encoderType);
}

void MainWindow::on_ComboBoxFPS_currentTextChanged()
{
    allocateMemory();
    QByteArray inBytes;
    const char *cStrData;

    inBytes = ui->ComboBoxFPS->currentText().toUtf8();
    cStrData = inBytes.constData();

    GST_DEBUG ("\nstr %s ",cStrData);

    gst->qtVars->fps = atoi(cStrData);
    GST_DEBUG ("\nFps = ",gst->qtVars->fps);

}

void MainWindow::on_ButtonFileSelect_clicked()
{
    allocateMemory();
    QString filename = QFileDialog::getSaveFileName(this , tr("open file"),"C://",tr("Videos (*.avi *.mp4 )"));

    QByteArray inBytes;
    const char *cStrData;

    inBytes = filename.toUtf8();
    cStrData = inBytes.constData();

    GST_DEBUG ("\nstr %s",cStrData);
    strcpy(gst->fileLocation, cStrData);
    ui->lineEdit_5->setText(cStrData);
}
void  MainWindow::showTime()
{
    long long int hour = (( ui->dateTimeEdit->time().hour() )* 3600 * 1000 );
    long long int min  = ((ui->dateTimeEdit->time().minute() )*60*1000);
    long long int sec  = ((ui->dateTimeEdit->time().second())*1000);

    long long int time_text;
    time_text = ( hour + min + sec )- ui->dateTimeEdit->time().elapsed();
    if( time_text == 0 )
        flag = 0;

}

void MainWindow::on_ButtonStart_clicked()
{
    allocateMemory();
    //start pipeline here
    GST_DEBUG ("\nstart pressed\n");
    ui->ButtonStart->setDisabled(true);
    ui->ButtonStop->setEnabled(true);

    QByteArray inBytes;
    const char *cStrData;
    inBytes = ui->LineEditTopX->text().toUtf8();
    cStrData = inBytes.constData();

    GST_DEBUG ("\nYour X coordinate : %s",cStrData);
    gst->qtVars->topLeftX = atoi(cStrData);
    GST_DEBUG ("\nTopLeftX coordinate : %d ",gst->qtVars->topLeftX);


    inBytes = ui->lineEdit_3->text().toUtf8();
    cStrData = inBytes.constData();
    GST_DEBUG ("\nYour Y coordinate : %s",cStrData);
    gst->qtVars->topLeftY = atoi(cStrData);
    GST_DEBUG ("\nTopLeftY coordinate : %d",gst->qtVars->topLeftY);


    inBytes = ui->lineEdit_2->text().toUtf8();
    cStrData = inBytes.constData();
    GST_DEBUG ("\nHeigth : %s",cStrData);
    gst->qtVars->height = atoi(cStrData);
    GST_DEBUG ("\nHeigth : %d",gst->qtVars->height);

    inBytes = ui->lineEdit_4->text().toUtf8();
    cStrData = inBytes.constData();
    GST_DEBUG ("\nWidth: %s",cStrData);
    gst->qtVars->width = atoi(cStrData);
    GST_DEBUG ("\nWidth : %d",gst->qtVars->width);

    gst->timer = new QTimer(this);
    connect(gst->timer, SIGNAL(timeout()), this, SLOT(showTime()));
    gst->timer->start();

    if( flag == 0 )
    {
        gst->timer->stop();
        stop_pipeline(gst);
    }
    start_pipeline( gst );
}

void MainWindow::on_ButtonStop_clicked()
{
    GST_DEBUG ("\nstop pressed\n");
    ui->ButtonStop->setDisabled(true);
    ui->ButtonStart->setEnabled(true);

    stop_pipeline( gst );
    gst->timer->stop();
    int temp = ui->dateTimeEdit->time().elapsed();
    GST_DEBUG ("\nElapsed Time : %d",temp);
    ui->LabelContainerFormat_4->clear();
    gst->cmd = "";
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
        stateFlag = 1;
        ret = gst_element_set_state( gst->source, GST_STATE_PAUSED );
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            GST_DEBUG ("failed to change state ret = %d\n",ret);
            return -1;
        }
        ret = gst_element_set_state ( gst->source, GST_STATE_READY);
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            GST_DEBUG ("failed to change state ret = %d\n",ret);
            return -1;
        }
        ret = gst_element_set_state( gst->source, GST_STATE_NULL );
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            GST_DEBUG ("failed to change state ret = %d\n",ret);
            return -1;
        }

        ret = gst_element_set_state( gst->pipeline, GST_STATE_PAUSED );
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            GST_DEBUG ("failed to change state ret = %d\n",ret);
            return -1;
        }
        ret = gst_element_set_state( gst->pipeline, GST_STATE_READY);
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            GST_DEBUG ("failed to change state ret = %d\n",ret);
            return -1;
        }
        ret = gst_element_set_state( gst->pipeline, GST_STATE_NULL );
        if( ret == GST_STATE_CHANGE_FAILURE )
        {
            GST_DEBUG ("failed to change state ret = %d\n",ret);
            return -1;
        }
    }
    else if (state == (GstStateChangeReturn) GST_STATE_PLAYING)
    {

        ret = gst_element_set_state ( gst->pipeline, GST_STATE_READY);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            GST_DEBUG ("failed to change state ret = %d\n",ret);
            return -1;
        }
        ret = gst_element_set_state ( gst->pipeline, GST_STATE_PAUSED);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            GST_DEBUG ("failed to change state ret = %d\n",ret);
            return -1;
        }
        ret = gst_element_set_state ( gst->pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            GST_DEBUG ("failed to change state ret = %d\n",ret);
            return -1;
        }
    }
    return 0;
}

static gboolean my_bus_callback( GstBus* bus, GstMessage *message, gpointer data )
{
    Gst* gst = ( Gst* ) data;
    switch( GST_MESSAGE_TYPE (message) )
    {
        case GST_MESSAGE_ERROR:
        {
            GST_DEBUG ("\nGot %s message\n", GST_MESSAGE_TYPE_NAME (message));
            GError *err;
            gchar *debug;
            gst_message_parse_error( message, &err, &debug );
            GST_DEBUG ("\nError: %s\n", err->message);
            g_error_free (err);
            g_free (debug);
            g_main_loop_quit ( gst->loop );
            break;
        }
        case GST_MESSAGE_EOS:
        {
            GST_DEBUG ("\nGot %s message\n", GST_MESSAGE_TYPE_NAME (message));
            /* end-of-stream */
            g_main_loop_quit ( gst->loop );
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
    GST_DEBUG ("\nStart create elements\n");
    GstElement *encoder;
    GstElement *muxer;

    if ( location == NULL )
    {
        GST_DEBUG ("\nAllocating memory to location\n");
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
        GST_DEBUG ("\nCreating pipeline\n");
        gst->pipeline = gst_pipeline_new( "pipeline" );
    }

   if (!gst->source)
    {
        GST_DEBUG ("\nCreating source\n");
        gst->source = gst_element_factory_make( "gdiscreencapsrc",    "videotestsrc" );
        gst->cmd = " gdiscreencapsrc ";
        gst->cmd += " ! ";
    }
    else
    {
       gst->cmd = " gdiscreencapsrc ";
       gst->cmd += " ! ";
    }

    if (!gst->filter)
    {
        GST_DEBUG ("\nCreating filter\n");
        gst->filter = gst_element_factory_make( "capsfilter", "filter" );
        /* Set up elements */
        g_object_set( G_OBJECT ( gst->filter ), "caps", gst->video_caps, NULL );
        gst->cmd += " capsfilter ";
        gst->cmd += " ! ";
    }
    else
    {
        gst->cmd += " capsfilter ";
        gst->cmd += " ! ";
    }

    if (!gst->ffmpegcolorspace)
    {
        GST_DEBUG ("\nCreating ffmpegcolorspace\n");
        gst->ffmpegcolorspace = gst_element_factory_make( "ffmpegcolorspace", "ffmpegcolorspace" );
        gst->cmd += " ffmpegcolorspace ";
        gst->cmd += " ! ";
    }
    else
    {
        gst->cmd += " ffmpegcolorspace ";
        gst->cmd += " ! ";
    }

    /* Create Encoder */
    encoder = gst_bin_get_by_name (GST_BIN (gst->pipeline), "encoder");
    if (encoder)
    {
        GST_DEBUG ("\nRemoving encoder\n");
        gst_bin_remove(GST_BIN (gst->pipeline), encoder);
        gst_object_unref (GST_OBJECT (gst->encoder));
    }

    GST_DEBUG ("\nCreating encoder\n");
    GST_DEBUG ("\nEncoder type in create_elements %s\n", gst->qtVars->encoderType );
    if( strcmp( gst->qtVars->encoderType, "Encoder_H264" ) == 0 )
    {
        gst->encoder = gst_element_factory_make( "x264enc",    "encoder" );
        gst->cmd += " x264enc ";
        gst->cmd += " ! ";
    }
    else
    {
        gst->encoder = gst_element_factory_make( "x264enc",    "encoder" );
        gst->cmd += " x264enc ";
        gst->cmd += " ! ";
    }

    if (encoder)
    {
        GST_DEBUG ("\nAdding encoder\n");
        gst_bin_add(GST_BIN (gst->pipeline), gst->encoder);
    }

    /* Create Muxer */
    muxer = gst_bin_get_by_name (GST_BIN (gst->pipeline), "mux");
    if (muxer)
    {
        GST_DEBUG ("\nRemoving muxer\n");
        gst_bin_remove(GST_BIN (gst->pipeline), muxer);
        gst_object_unref (GST_OBJECT (gst->muxer));
    }

    GST_DEBUG ("\nCreating muxer\n");
    if( strcmp( gst->qtVars->containerFormat, "Container_AVI" ) == 0 )
    {
        gst->muxer = gst_element_factory_make( "avimux", "mux" );
        gst->cmd += " avimux ";
        gst->cmd += " ! ";
    }
    else if( strcmp( gst->qtVars->containerFormat, "Container_mp4" ) == 0 )
    {
        GST_DEBUG ("\n creating mpeg4mux\n");
        gst->muxer = gst_element_factory_make( "mp4mux", "mux" );
        gst->cmd += " mp4mux ";
        gst->cmd += " ! ";
    }
    else
    {
        gst->muxer = gst_element_factory_make( "avimux", "mux" );
        gst->cmd += " avimux ";
        gst->cmd += " ! ";
    }

    GST_DEBUG ("muxer type in create_elements %s\n", gst_element_get_name(gst->muxer) );

    if (encoder)
    {
        GST_DEBUG ("\nAdding Muxer\n");
        gst_bin_add(GST_BIN (gst->pipeline), gst->muxer);
    }

   if (!gst->sink)
    {
        GST_DEBUG ("\nCreating sink\n");
        gst->sink = gst_element_factory_make( "filesink", "filesink" );
        GST_DEBUG ("sink type in create_elements %s\n", gst_element_get_name(gst->sink) );

        /* we set the input filename to the source element */
        g_object_set( G_OBJECT( gst->sink ), "location", gst->fileLocation, NULL );
        GST_DEBUG ("\nLocation = %s\n",gst->fileLocation);
        gst->cmd += "filesink";
    }
    else
       gst->cmd += "filesink";

    if (!gst->qtVars->fps)
        gst->qtVars->fps = DEFAULT_FPS;

    if (!gst->video_caps)
    {
        GST_DEBUG ("\nCreating video_caps\n");
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

    GST_DEBUG ("(%dx%d) (%dx%d)",gst->qtVars->topLeftX,gst->qtVars->topLeftY,
                    gst->qtVars->width,gst->qtVars->height);

    GST_DEBUG ("\nEnd of create elements\n");

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
    if( ret == false )
    {
        GST_DEBUG ("Linking error, Linking individual components\n");
        if (gst_element_link( gst->encoder, gst->muxer ) == false)
            return false;
        if (gst_element_link( gst->muxer, gst->sink ) == false)
            return false;
        if (gst_element_link( gst->ffmpegcolorspace, gst->encoder) == false)
            return false;
        GST_DEBUG ("Linking Done\n");
    }
    else
        GST_DEBUG ("Linking Done\n");

   g_print( "Command : %s" , gst->cmd.c_str() );
   ui->LabelContainerFormat_4->setText(gst->cmd.c_str());

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
    /*Gst* gst = ( Gst* ) data;
    double difft = 0;*/

    g_print ("Stop Recording ...\n");

    if( state_handler( gst, GST_STATE_NULL) !=0 )
    return -1;
    g_main_loop_quit ( gst->loop );
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

    g_print ("Start Recording ...\n");
    g_main_loop_run ( gst->loop);

    return 0;
}


void MainWindow::on_lineEdit_5_editingFinished()
{
    QByteArray inBytes;
    const char *temp;
    inBytes = ui->lineEdit_5->text().toUtf8();
    temp = inBytes.constData();
    GST_DEBUG ("\nEntered Filename : %s",temp);

    FILE* fp;
    bool ret;
    stat( temp, &st );
    if( S_ISDIR( st.st_mode )  )
    {
      ui->lineEdit_5->setText("***It is directory enter path with file name***");
    }
    else
    {
        ret = fileExists( temp );
        if( ret )
        {
          strcpy( gst->fileLocation, temp );
        }
        else
        {
            fp = fopen(temp, "wb");
            if( fp == NULL )
            {
                GST_ERROR ( "ERROR :: incorrect file.\n" );
            }
            strcpy( gst->fileLocation, temp );
        }

     }
}

void MainWindow::closeEvent (QCloseEvent *event)
{

    if( !stateFlag )
    {
           if( state_handler( gst, GST_STATE_NULL) !=0 )
                   GST_ERROR ( "ERROR :: statehandler error.\n" );
           g_main_loop_quit ( gst->loop );
    }
    int temp = ui->dateTimeEdit->time().elapsed();
    GST_DEBUG ("\nElapsed Time : %d",temp);
    ui->LabelContainerFormat_4->clear();
    gst->cmd = "";
}
