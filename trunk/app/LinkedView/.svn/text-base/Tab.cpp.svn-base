/*****************************************************************************/
/**
 *  @file   Tab.cpp
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
#include "Tab.h"
#include "Model.h"
#include "View.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QTableView>
#include <QFileDialog>
#include <kvs/qt/Qt>


/*===========================================================================*/
/**
 *  @brief  Constructs a new Tab class.
 *  @param  model [in] pointer to the model
 */
/*===========================================================================*/
Tab::Tab(
    Model* model,
    ParallelView* parallel_view,
    VolumeView* volume_view,
    SliceView* slice_view ):
    m_model( model ),
    m_parallel_view( parallel_view ),
    m_volume_view( volume_view ),
    m_slice_view( slice_view )
{
    m_variable_table = new QStandardItemModel();
    m_variable_table->setHorizontalHeaderItem( 0, new QStandardItem("Names") );
    m_variable_table->setHorizontalHeaderItem( 1, new QStandardItem("Description") );

    m_time_step_table = new QStandardItemModel();
    m_time_step_table->setHorizontalHeaderItem( 0, new QStandardItem("Year") );
    m_time_step_table->setHorizontalHeaderItem( 1, new QStandardItem("Month") );
    m_time_step_table->setHorizontalHeaderItem( 2, new QStandardItem("Day") );

    m_variable_table_view = new QTableView();
    m_variable_table_view->setModel( m_variable_table );
    m_variable_table_view->setShowGrid( false );
    m_variable_table_view->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_variable_table_view->setSelectionMode( QAbstractItemView::SingleSelection );
    m_variable_table_view->setEditTriggers( QAbstractItemView::NoEditTriggers );
    m_variable_table_view->resizeRowsToContents();
    m_variable_table_view->verticalHeader()->hide();
#if ( KVS_QT_VERSION >= 5 )
    m_variable_table_view->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
#else
    m_variable_table_view->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
#endif

    m_time_step_table_view = new QTableView();
    m_time_step_table_view->setModel( m_time_step_table );
    m_time_step_table_view->setColumnWidth( 0, 70 );
    m_time_step_table_view->setShowGrid( false );
    m_time_step_table_view->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_time_step_table_view->setEditTriggers( QAbstractItemView::NoEditTriggers );
    m_time_step_table_view->resizeRowsToContents();
    m_time_step_table_view->verticalHeader()->hide();
#if ( KVS_QT_VERSION >= 5 )
    m_time_step_table_view->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
#else
    m_time_step_table_view->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
#endif

    QWidget* tab1 = this->create_data_tab();
    QWidget* tab2 = this->create_volume_tab();
    QWidget* tab3 = this->create_slice_tab();
    QWidget* tab4 = this->create_parallel_tab();

    QTabWidget::addTab( tab1, "Data" );
    QTabWidget::addTab( tab2, "Volume" );
    QTabWidget::addTab( tab3, "Slice" );
    QTabWidget::addTab( tab4, "Parallel" );
    QTabWidget::setTabShape( QTabWidget::Triangular );
}

/*===========================================================================*/
/**
 *  @brief  Creates a data tab.
 *  @return pointer to the tab widget
 */
/*===========================================================================*/
QWidget* Tab::create_data_tab()
{
    QWidget* tab = new QWidget();

    QGridLayout* file_layout = new QGridLayout();
    {
        QPushButton* button = new QPushButton( "Open DB ..." );
        QObject::connect( button, SIGNAL( clicked() ), this, SLOT( open() ) );
        file_layout->addWidget( button );
    }

    QGridLayout* variables_layout = new QGridLayout();
    {
        variables_layout->addWidget( new QLabel( "Variables:" ), 0, 0 );
        variables_layout->addWidget( m_variable_table_view, 1, 0 );
    }

    QGridLayout* time_steps_layout = new QGridLayout();
    {
        time_steps_layout->addWidget( new QLabel( "Time steps:" ), 0, 0 );
        time_steps_layout->addWidget( m_time_step_table_view, 1, 0 );
    }

    QGridLayout* load_layout = new QGridLayout();
    {
        QPushButton* button = new QPushButton( "Load ..." );
        QObject::connect( button, SIGNAL( clicked() ), this, SLOT( load() ) );
        load_layout->addWidget( button );
    }

    QVBoxLayout* main_layout = new QVBoxLayout( tab );
    main_layout->addLayout( file_layout );
    main_layout->addLayout( variables_layout );
    main_layout->addLayout( time_steps_layout );
    main_layout->addLayout( load_layout );
    main_layout->addStretch(1);

    return tab;
}

/*===========================================================================*/
/**
 *  @brief  Creates a volume tab.
 *  @return pointer to the tab widget
 */
/*===========================================================================*/
QWidget* Tab::create_volume_tab()
{
    QWidget* tab = new QWidget();
    return tab;
}

/*===========================================================================*/
/**
 *  @brief  Creates a slice tab.
 *  @return pointer to the tab widget
 */
/*===========================================================================*/
QWidget* Tab::create_slice_tab()
{
    QWidget* tab = new QWidget();
    return tab;
}

/*===========================================================================*/
/**
 *  @brief  Creates a parallel tab.
 *  @return pointer to the tab widget
 */
/*===========================================================================*/
QWidget* Tab::create_parallel_tab()
{
    QWidget* tab = new QWidget();
    return tab;
}

/*===========================================================================*/
/**
 *  @brief  Open data file.
 */
/*===========================================================================*/
void Tab::open()
{
    // Read defualt directory name from 'path.data' file.
    QDir dir = QDir::home();
    QFile file( "path.dat" );
    if ( file.exists() )
    {
        file.open( QFile::ReadOnly | QFile::Text );
        char path[ 256 ];
        file.readLine( path, sizeof( path ) );
        file.close();
        dir.setPath( QString( path ) );
    }

    // Open dialog.
    QFileDialog* dialog = new QFileDialog( this, tr( "Open file ..." ), dir.path() );
    dialog->setAcceptMode( QFileDialog::AcceptOpen );
    dialog->setViewMode( QFileDialog::Detail );
    if ( dialog->exec() )
    {
        // Write the directory name of read data file to 'path.dat' file.
        file.open( QFile::WriteOnly | QFile::Text );
        file.write( dialog->directory().absolutePath().toStdString().c_str(), dialog->directory().absolutePath().size() );
        file.close();

        QStringList filenames = dialog->selectedFiles();
        QString filename = filenames.at(0);
        QFile data_file( filename );
        data_file.open( QFile::ReadOnly | QFile::Text );

        QTextStream in( &data_file );
        while ( !in.atEnd() )
        {
            QString f = in.readLine();
            f = dialog->directory().absolutePath() + QDir::separator() + f;

            m_model->read( f.toStdString() );
            if ( m_model->dataBase().size() > 0 )
            {
                typedef kvs::GrADS::GriddedBinaryDataFileList TimeList;
                typedef std::list<kvs::grads::Vars::Var> VarList;

                size_t i = m_model->dataBase().size() - 1;
                const DataBase::Data& data = m_model->dataBase().data(i);

                if ( i == 0 )
                {
                    TimeList::const_iterator time = data.dataList().begin();
                    TimeList::const_iterator last_time = data.dataList().end();
                    size_t j = 0;
                    while ( time != last_time )
                    {
                        QString year; year.setNum( time->date().year );
                        QString month; month.setNum( time->date().month );
                        QString day; day.setNum( time->date().day );
                        m_time_step_table->setItem( j, 0, new QStandardItem( year ) );
                        m_time_step_table->setItem( j, 1, new QStandardItem( month ) );
                        m_time_step_table->setItem( j, 2, new QStandardItem( day ) );

                        time++;
                        j++;
                    }
                }

                VarList::const_iterator var = data.dataDescriptor().vars().values.begin();
                VarList::const_iterator last_var = data.dataDescriptor().vars().values.end();
                size_t k = i;
                while ( var != last_var )
                {
                    m_variable_table->setItem( k, 0, new QStandardItem( var->varname.c_str() ) );
                    m_variable_table->setItem( k, 1, new QStandardItem( var->description.c_str() ) );

                    var++;
                    k++;
                }
            }
        }
    }

    delete dialog;
}

void Tab::load()
{
    if ( m_model->dataBase().size() > 0 )
    {
        int vrow = m_variable_table_view->currentIndex().row();
        if ( vrow < 0 )
        {
            QMessageBox m;
            m.setText( "Variable has not been selected." );
            m.exec();
            return;
        }

        int trow = m_time_step_table_view->currentIndex().row();
        if ( trow < 0 )
        {
            QMessageBox m;
            m.setText( "Time step has not been selected." );
            m.exec();
            return;
        }

        std::string varname = m_variable_table->item( vrow )->text().toStdString();
        size_t tindex = static_cast<size_t>( trow );

        if ( !m_model->importVolumeObject( varname, tindex ) )
        {
            QMessageBox m;
            m.setText( "Volume data cannot been imported." );
            m.exec();
            return;
        }

        if ( !m_model->importTableObject( tindex ) )
        {
            QMessageBox m;
            m.setText( "Table data cannot been imported." );
            m.exec();
            return;
        }

        const size_t index = m_model->volumeObject()->resolution().z() - 1; // sea surface level
        const float ignore_value = m_model->dataBase().data( vrow ).dataDescriptor().undef().value;

        m_slice_view->setIndex( index );
        m_slice_view->setIgnoreValue( ignore_value );
        m_slice_view->insertObject( m_model->volumeObject() );
        m_slice_view->redraw();

        kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeObject();
        volume->shallowCopy( *(m_model->volumeObject()) );
        m_volume_view->insertObject( volume );
//        m_volume_view->insertObject( m_model->volumeObject() );
        m_volume_view->redraw();

        m_parallel_view->setActiveAxis( vrow );
        m_parallel_view->insertObject( m_model->tableObject() );
        m_parallel_view->redraw();
    }
    else
    {
        QMessageBox msg;
        msg.setText( "Database has not been opened." );
        msg.exec();
    }
}
