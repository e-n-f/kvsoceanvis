/*****************************************************************************/
/**
 *  @file   ParallelCoordinates.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: ParallelCoordinates.cpp 342 2013-06-12 04:03:10Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include "ParallelCoordinates.h"
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/CommandLine>
#include <kvs/File>
#include <kvs/TableObject>
#include <kvs/TableImporter>
#include <kvs/ParallelCoordinatesRenderer>
#include <kvs/glut/ParallelAxis>
#include <util/CreateRandomTable.h>
#include <util/CreateColorMap.h>
#include <util/Event.h>
#include <util/CsvReader.h>


using namespace kvsoceanvis;

typedef kvs::glut::Application Application;
typedef kvs::glut::Screen Screen;
typedef kvs::TableObject Object;
typedef kvs::ParallelCoordinatesRenderer Renderer;
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
 *  @brief  Constructs a new ParallelCoordinates command class.
 *  @param  argc [in] argument count
 *  @param  argv [in] argument values
 */
/*===========================================================================*/
ParallelCoordinates::ParallelCoordinates( int argc, char** argv ):
    Command( argc, argv )
{
}

/*===========================================================================*/
/**
 *  @brief  Executes parallel coordinates command.
 *  @return true if the process is done successfully
 */
/*===========================================================================*/
int ParallelCoordinates::exec( void )
{
    Application app( BaseClass::argc(), BaseClass::argv() );

    const std::string name = ParallelCoordinates::CommandName();
    const std::string desc = ParallelCoordinates::CommandDescription();
    const std::string command = std::string( BaseClass::argv(0) ) + " -" + name;
    kvs::CommandLine commandline( BaseClass::argc(), BaseClass::argv(), command );
    commandline.addHelpOption("help");
    commandline.addOption( name, desc + "." );
    commandline.addOption( "verbose", "Verbose output.", 0, false );
    commandline.addOption( "antialiasing", "Enable anti-aliasing. (optional)", 0, false );
    commandline.addOption( "opacity", "Opacity value. (default: 255)", 1, false );
    commandline.addOption( "bgcolor", "Background color. (default: 212 221 229)", 3, false );
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
        if ( kvs::File( filename ).extension() == "csv" )
        {
            object = new util::CsvReader( filename );
        }
        else
        {
            object = new kvs::TableImporter( filename );
        }
        if ( verbose ) std::cout << "done." << std::endl;
    }
    else
    {
        if ( verbose ) std::cout << "Creating table object ... " << std::flush;
        const size_t nsamples = 500;
        const size_t naxes = 10;
        object = new kvs::TableObject( util::CreateRandomTable( nsamples, naxes ) );
        if ( verbose ) std::cout << "done." << std::endl;
    }
    object->setName( object_name );

    if ( verbose )
    {
        const size_t ncolumns = object->numberOfColumns();
        const size_t nrows = object->numberOfRows();
        std::cout << "  Number of columns: " << ncolumns << std::endl;
        std::cout << "  Number of nrows: " << nrows << std::endl;
    }

    const int top_margin = 40;
    const int bottom_margin = 40;
    const int left_margin = 40;
    const int right_margin = 40;
    const float line_width = 1.5f;
    const int opacity = commandline.hasOption("opacity") ? commandline.optionValue<int>("opacity") : 255;
    const std::string renderer_name("PCs");
    Renderer* renderer = new Renderer();
    renderer->setName( renderer_name );
    renderer->setTopMargin( top_margin );
    renderer->setBottomMargin( bottom_margin );
    renderer->setLeftMargin( left_margin );
    renderer->setRightMargin( right_margin );
    renderer->setLineWidth( line_width );
    renderer->setLineOpacity( opacity );
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
