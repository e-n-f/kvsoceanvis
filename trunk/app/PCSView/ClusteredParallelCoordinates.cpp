/*****************************************************************************/
/**
 *  @file   ClusteredParallelCoordinates.cpp
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
#include "ClusteredParallelCoordinates.h"
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/CommandLine>
#include <kvs/File>
#include <kvs/Timer>
#include <kvs/TableObject>
#include <kvs/TableImporter>
#include <kvs/ParallelCoordinatesRenderer>
#include <kvs/glut/ParallelAxis>

#include <pcs/KMeansClustering.h>
#include <pcs/FastKMeansClustering.h>
#include <pcs/ClusterMapObject.h>
#include <pcs/ClusterMapping.h>
#include <pcs/ClusterMappedParallelCoordinatesRenderer.h>
#include <util/CreateRandomTable.h>
#include <util/CreateColorMap.h>
#include <util/Event.h>
#include <util/CsvReader.h>

using namespace kvsoceanvis;

typedef kvs::glut::Application Application;
typedef kvs::glut::Screen Screen;
typedef pcs::ClusterMapObject Object;
typedef pcs::ClusterMappedParallelCoordinatesRenderer Renderer;
typedef kvs::glut::ParallelAxis Axis;

typedef util::Event::Parameter Parameter;
typedef util::Event::KeyPress<Object,Renderer> KeyPress;
typedef util::Event::MousePress<Object,Renderer> MousePress;
typedef util::Event::MouseDoubleClick<Object,Renderer> MouseDoubleClick;
typedef util::Event::MouseMove<Object,Renderer> MouseMove;
typedef util::Event::MouseRelease<Object,Renderer> MouseRelease;
typedef util::Event::Paint<Object,Renderer> Paint;


/*===========================================================================*/
/**
 *  @brief  Constructs a new ClusteredParallelCoordinates command class.
 *  @param  argc [in] argumnet count
 *  @param  argv [in] argument values
 */
/*===========================================================================*/
ClusteredParallelCoordinates::ClusteredParallelCoordinates( int argc, char** argv ):
    Command( argc, argv )
{
}

/*===========================================================================*/
/**
 *  @brief  Executes multi-binned parallel coordinates command.
 *  @return true if the process is done successfully
 */
/*===========================================================================*/
int ClusteredParallelCoordinates::exec( void )
{
    Application app( BaseClass::argc(), BaseClass::argv() );

    const std::string name = ClusteredParallelCoordinates::CommandName();
    const std::string desc = ClusteredParallelCoordinates::CommandDescription();
    const std::string command = std::string( BaseClass::argv(0) ) + " -" + name;
    kvs::CommandLine commandline( BaseClass::argc(), BaseClass::argv(), command );
    commandline.addHelpOption("help");
    commandline.addOption( name, desc + "." );
    commandline.addOption( "verbose", "Verbose output.", 0, false );
    commandline.addOption( "antialiasing", "Enable anti-aliasing. (optional)", 0, false );
    commandline.addOption( "opacity", "Opacity value. (default: 255)", 1, false );
    commandline.addOption( "bgcolor", "Background color. (default: 212 221 229)", 3, false );
    commandline.addOption( "nclusters", "Number of clusters for table. (default: 10)", 1, false );
    commandline.addOption( "fast", "Hamerly's fast k-means algorithm. (optional)", 0, false );
    commandline.addValue( "input data file", false );
    if ( !commandline.parse() ) return( false );

    // Verbose mode.
    const bool verbose = commandline.hasOption("verbose");

    const std::string object_name("Table");
    Object* object = NULL;
    if ( commandline.hasValues() )
    {
        const std::string filename( commandline.value<std::string>() );
        if ( !kvs::File( filename ).isExisted() )
        {
            kvsMessageError( "%s is not existed.", filename.c_str() );
            return( false );
        }

        if ( verbose ) std::cout << "Reading " << filename << " ... " << std::flush;
        kvs::TableObject* table = NULL;
        if ( kvs::File( filename ).extension() == "csv" )
        {
            table = new util::CsvReader( filename );
        }
        else
        {
            table = new kvs::TableImporter( filename );
        }
        if ( verbose ) std::cout << "done." << std::endl;

        // Number of clusters.
        size_t nclusters = 10;
        if ( commandline.hasOption("nclusters") )
        {
            nclusters = commandline.optionValue<size_t>("nclusters");
        }

        if ( verbose ) std::cout << "Clustering ... " << std::flush;
        kvs::Timer timer( kvs::Timer::Start );
        kvs::TableObject* clustered_table = NULL;
        if ( commandline.hasOption("fast") ) clustered_table = new pcs::FastKMeansClustering( table, nclusters );
        else clustered_table = new pcs::KMeansClustering( table, nclusters );
        timer.stop();
        delete table;
        if ( verbose ) std::cout << "done. [" << timer.msec() << " msec]" << std::endl;

        if ( verbose ) std::cout << "Mapping ... " << std::flush;
        object = new pcs::ClusterMapping( clustered_table );
        delete clustered_table;
        if ( verbose ) std::cout << "done." << std::endl;
    }
    else
    {
        if ( verbose ) std::cout << "Creating table object ... " << std::flush;
        const size_t nsamples = 100;
        const size_t naxes = 10;
        kvs::TableObject* table = new kvs::TableObject( util::CreateRandomTable( nsamples, naxes ) );
        if ( verbose ) std::cout << "done." << std::endl;

        // Number of clusters.
        size_t nclusters = 10;
        if ( commandline.hasOption("nclusters") )
        {
            nclusters = commandline.optionValue<size_t>("nclusters");
        }

        if ( verbose ) std::cout << "Clustering ... " << std::flush;
        kvs::Timer timer( kvs::Timer::Start );
        kvs::TableObject* clustered_table = new pcs::KMeansClustering( table, nclusters );
        timer.stop();
        delete table;
        if ( verbose ) std::cout << "done. [" << timer.msec() << " msec]" << std::endl;

        if ( verbose ) std::cout << "Mapping ... " << std::flush;
        object = new pcs::ClusterMapping( clustered_table );
        delete clustered_table;
        if ( verbose ) std::cout << "done." << std::endl;
    }
    object->setName( object_name );

    if ( verbose )
    {
        std::cout << "  Number of axes: " << object->naxes() << std::endl;
        std::cout << "  Number of sampling points: " << object->npoints() << std::endl;
        std::cout << "  Number of clusters: " << object->nclusters() << std::endl;
    }

    const int top_margin = 40;
    const int bottom_margin = 40;
    const int left_margin = 40;
    const int right_margin = 40;
    const float cluster_width = 1.0f;
    const int opacity = commandline.hasOption("opacity") ? commandline.optionValue<int>("opacity") : 255;
    const std::string renderer_name("PCs");
    Renderer* renderer = new Renderer();
    renderer->setName( renderer_name );
    renderer->setTopMargin( top_margin );
    renderer->setBottomMargin( bottom_margin );
    renderer->setLeftMargin( left_margin );
    renderer->setRightMargin( right_margin );
    renderer->setClusterEdgeWidth( cluster_width );
    renderer->setClusterOpacity( opacity );
    if ( commandline.hasOption("antialiasing") ) renderer->enableAntiAliasing();

    const float axis_width = 4.0f;
    const kvs::RGBColor axis_color = kvs::RGBColor( 0, 0, 0 );
    const kvs::RGBColor value_color = kvs::RGBColor( 0, 0, 0 );
    const kvs::RGBColor label_color = kvs::RGBColor( 200, 100, 100 );
    Axis* axis = new Axis();
    axis->setTopMargin( top_margin );
    axis->setBottomMargin( bottom_margin );
    axis->setLeftMargin( left_margin );
    axis->setRightMargin( right_margin );
    axis->setAxisWidth( axis_width );
    axis->setAxisColor( axis_color);
    axis->setValueColor( value_color );
    axis->setLabelColor( label_color );

    Parameter parameter;
    KeyPress key_press( parameter, object_name, renderer_name );
    MousePress mouse_press( parameter, object_name, renderer_name );
    MouseDoubleClick mouse_double_click( parameter, object_name, renderer_name );
    MouseMove mouse_move( parameter, object_name, renderer_name );
    MouseRelease mouse_release( parameter, object_name, renderer_name );
    Paint paint( parameter, object_name, renderer_name );

    const int width = 1000;
    const int height = 250;
    Screen screen( &app );
    screen.setTitle( desc );
    screen.setSize( width, height );
    screen.registerObject( object, renderer );
    screen.registerObject( object, axis );
    screen.addEvent( &key_press );
    screen.addEvent( &mouse_press );
    screen.addEvent( &mouse_double_click );
    screen.addEvent( &mouse_move );
    screen.addEvent( &mouse_release );
    screen.addEvent( &paint );
    screen.show();

    if ( commandline.hasOption("bgcolor") )
    {
        const int r = commandline.optionValue<int>("bgcolor",0);
        const int g = commandline.optionValue<int>("bgcolor",1);
        const int b = commandline.optionValue<int>("bgcolor",2);
        screen.setBackgroundColor( kvs::RGBColor( r, g, b ) );
    }

    return( app.run() );
}
