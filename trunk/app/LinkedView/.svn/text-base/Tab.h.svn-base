/*****************************************************************************/
/**
 *  @file   Tab.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
#ifndef TAB_H_INCLUDE
#define TAB_H_INCLUDE

#include <QTabWidget>
#include "View.h"

class Model;
class View;
class QLineEdit;
class QStandardItemModel;
class QTableView;

/*===========================================================================*/
/**
 *  @brief  Tab class.
 */
/*===========================================================================*/
class Tab : public QTabWidget
{
    Q_OBJECT

private:

    Model* m_model;
    ParallelView* m_parallel_view;
    VolumeView* m_volume_view;
    SliceView* m_slice_view;
    QStandardItemModel* m_variable_table;
    QStandardItemModel* m_time_step_table;
    QTableView* m_variable_table_view;
    QTableView* m_time_step_table_view;

public:

    Tab( Model* model,
         ParallelView* parallel_view,
         VolumeView* volume_view,
         SliceView* slice_view );

private:

    QWidget* create_data_tab();
    QWidget* create_volume_tab();
    QWidget* create_slice_tab();
    QWidget* create_parallel_tab();

private slots:

    void open();
    void load();

//signals:
//    void set_filename( const QString& filename );
};

#endif // TAB_H_INCLUDE
