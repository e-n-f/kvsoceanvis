/*****************************************************************************/
/**
 *  @file   MainWindow.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: MainWindow.h 395 2014-10-24 09:00:30Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#ifndef MAIN_WINDOW_H_INCLUDE
#define MAIN_WINDOW_H_INCLUDE

#include <QMainWindow>

class Model;
class ParallelView;
class VolumeView;
class SliceView;
class Controller;
class Tab;
class QMenu;
class QToolBar;
class QAction;
namespace kvs { namespace qt { class Application; } }

/*===========================================================================*/
/**
 *  @brief  Main window class.
 */
/*===========================================================================*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:

    Model* m_model; ///< model component for dealing with the rendering objects
    ParallelView* m_parallel_view; ///< parallel coordinates view
    VolumeView* m_volume_view; ///< 3D volume view
    SliceView* m_slice_view; ///< 2D slice view
    Controller* m_controller; ///< controller component for the views
    Tab* m_tab; ///< tab widget

    QMenu* m_file_menu;
    QAction* m_open_action;
    QAction* m_save_action;
    QAction* m_home_action;
    QAction* m_snap_action;

public:

    MainWindow( kvs::qt::Application* app );
    virtual ~MainWindow();

private slots:

    void open();
    bool save();
    void home();
    void snap();

private:

    void create_action();
    void create_menu();
    void create_status_bar();
};

#endif // MAIN_WINDOW_H_INCLUDE
