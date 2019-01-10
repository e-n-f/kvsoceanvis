/*****************************************************************************/
/**
 *  @file   MainWindow.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: MainWindow.cpp 395 2014-10-24 09:00:30Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include "MainWindow.h"
#include "Model.h"
#include "View.h"
#include "Controller.h"
#include "Tab.h"
#include <kvs/PolygonObject>
#include <kvs/ColorMap>
#include <kvs/qt/Application>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QDockWidget>


/*===========================================================================*/
/**
 *  @brief  BottomDockWidget class.
 */
/*===========================================================================*/
class BottomDockWidget : public QDockWidget
{
public:

    BottomDockWidget( QWidget* parent, QWidget* widget, int width, int height ):
        QDockWidget( parent )
    {
        QDockWidget::setWidget( widget );
        QDockWidget::setAllowedAreas( Qt::BottomDockWidgetArea );
        QDockWidget::setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
        QDockWidget::setMinimumWidth( width );
        QDockWidget::setMinimumHeight( height );
    }

    void resizeEvent( QResizeEvent* event )
    {
#if ( KVS_QT_VERSION < 5 )
        QDockWidget::setMinimumWidth( 0 );
        QDockWidget::setMinimumHeight( 0 );
#endif
    }
};

/*===========================================================================*/
/**
 *  @brief  RightDockWidget class.
 */
/*===========================================================================*/
class RightDockWidget : public QDockWidget
{
public:

    RightDockWidget( QWidget* parent, QWidget* widget, int width ):
        QDockWidget( parent )
    {
        QDockWidget::setWidget( widget );
        QDockWidget::setAllowedAreas( Qt::RightDockWidgetArea );
        QDockWidget::setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
        QDockWidget::setMinimumWidth( width );
    }
};


/*===========================================================================*/
/**
 *  @brief  Constructs a new MainWindow class.
 *  @param  app [in] pointer to the application
 */
/*===========================================================================*/
MainWindow::MainWindow( kvs::qt::Application* app )
{
    const kvs::RGBColor screen_color( 212, 221, 229 );

    m_model = new Model();
    m_parallel_view = new ParallelView( app, screen_color );
    m_slice_view = new SliceView( app, screen_color );
    m_volume_view = new VolumeView( app, screen_color );
    m_controller = new Controller( m_model, m_parallel_view, m_volume_view, m_slice_view );
    m_tab = new Tab( m_model, m_parallel_view, m_volume_view, m_slice_view );

    BottomDockWidget* dock1 = new BottomDockWidget( this, m_parallel_view, 500, 250 );
    BottomDockWidget* dock2 = new BottomDockWidget( this, m_slice_view, 250, 250 );
    RightDockWidget* dock3 = new RightDockWidget( this, m_tab, 300 );

    QMainWindow::addDockWidget( Qt::BottomDockWidgetArea, dock1 );
    QMainWindow::addDockWidget( Qt::BottomDockWidgetArea, dock2 );
    QMainWindow::addDockWidget( Qt::RightDockWidgetArea, dock3 );
    QMainWindow::setCentralWidget( m_volume_view );
    QMainWindow::setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

    this->create_action();
    this->create_menu();
    this->create_status_bar();
}

/*===========================================================================*/
/**
 *  @brief  Destroys the MainWindow class.
 */
/*===========================================================================*/
MainWindow::~MainWindow()
{
    delete m_model;
    delete m_parallel_view;
    delete m_slice_view;
    delete m_volume_view;
    delete m_controller;
    delete m_tab;
    delete m_file_menu;
}

/*===========================================================================*/
/**
 *  @brief  Opens a file.
 */
/*===========================================================================*/
void MainWindow::open()
{
/*
    QString filename = QFileDialog::getOpenFileName( this );
    filename = QDir::toNativeSeparators( filename );
    if ( !filename.isEmpty() )
    {
        // The registered objects and renderers are erased.
        m_volume_view->clearRegisteredObjects();
        m_parallel_view->clearRegisteredObjects();

        m_model->importTableObject( filename.toStdString() );
        m_model->registerTableObject( m_model->tableObject(), m_parallel_view );

        const size_t left_margin = 40;
        const size_t right_margin = 40;
        const size_t naxes = m_model->tableObject()->ncolumns();
        const size_t active_axis = 3;
        m_controller->updateAxisSelector( left_margin, right_margin, naxes, active_axis );

        kvs::ColorMap cmap( 256 ); cmap.create();
        kvs::PolygonObject* polygon = m_model->extractExternalFaces( m_model->tableObject(), 3, cmap );
        m_model->registerPolygonObject( polygon, m_volume_view );

        m_parallel_view->redraw();
        m_volume_view->redraw();
    }
*/
}

bool MainWindow::save()
{
    return true;
}

void MainWindow::home()
{
}

void MainWindow::snap()
{
/*
    QString caption = tr("Save screenshot image");
    QString dir = QDir::currentPath();
    QString filter = tr("Image file (*.png *.bmp *.jpg)");
    QString filename = QFileDialog::getSaveFileName( this, caption, dir, filter );
    if ( !filename.isEmpty() )
    {
        kvs::ColorImage snapshot = m_parallel_view->camera()->snapshot();
        QImage image( snapshot.data().pointer(), snapshot.width(), snapshot.height(), QImage::Format_RGB888 );
        image.save( filename );
    }
    m_volume_view->show();
*/
}

void MainWindow::create_action()
{
    m_open_action = new QAction( QIcon(":/icons/open.png"), tr("&Open..."), this );
    m_open_action->setShortcuts( QKeySequence::Open );
    m_open_action->setStatusTip( tr("Open an existing file") );
    QObject::connect( m_open_action, SIGNAL( triggered() ), this, SLOT( open() ) );

/*
    m_save_action = new QAction( QIcon(":/icons/save.png"), tr("&Save..."), this );
    m_save_action->setShortcuts( QKeySequence::Save );
    m_save_action->setStatusTip( tr("Save the object to disk") );
    QObject::connect( m_save_action, SIGNAL( triggered() ), this, SLOT( save() ) );

    m_home_action = new QAction( QIcon(":/icons/home.png"), tr("&Home..."), this );
    m_home_action->setShortcuts( QKeySequence::Refresh );
    m_home_action->setStatusTip( tr("Reset to the home position") );
    QObject::connect( m_home_action, SIGNAL( triggered() ), this, SLOT( home() ) );
*/

    m_snap_action = new QAction( QIcon(":/icons/snap.png"), tr("&Snapshot..."), this );
    m_snap_action->setStatusTip( tr("Take a screen snapshot") );
    QObject::connect( m_snap_action, SIGNAL( triggered() ), this, SLOT( snap() ) );
}

void MainWindow::create_menu()
{
    m_file_menu = QMainWindow::menuBar()->addMenu( tr("&File") );
    m_file_menu->addAction( m_open_action );
//    m_file_menu->addAction( m_save_action );
//    m_file_menu->addAction( m_home_action );
    m_file_menu->addAction( m_snap_action );
}

void MainWindow::create_status_bar()
{
    statusBar()->showMessage( tr("Ready") );
}
