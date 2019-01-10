/*****************************************************************************/
/**
 *  @file   MultiBinnedParallelCoordinatesCommand.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: MultiBinnedParallelCoordinates.cpp 342 2013-06-12 04:03:10Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include "MultiBinnedParallelCoordinates.h"
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/CommandLine>
#include <kvs/File>
#include <kvs/Tokenizer>
#include <kvs/TableObject>
#include <kvs/TableImporter>
#include <kvs/ParallelCoordinatesRenderer>
#include <kvs/glut/ParallelAxis>

#include <pcs/MultiBinMapObject.h>
#include <pcs/MultiBinMapObjectReader.h>
#include <pcs/MultiBinMapping.h>
#include <pcs/MultiBinMappedParallelCoordinatesRenderer.h>
#include <util/CreateRandomTable.h>
#include <util/CreateColorMap.h>
#include <util/Event.h>
#include <util/CsvReader.h>

using namespace kvsoceanvis;

typedef kvs::glut::Application Application;
typedef kvs::glut::Screen Screen;
typedef pcs::MultiBinMapObject Object;
typedef pcs::MultiBinMappedParallelCoordinatesRenderer Renderer;
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
 *  @brief  Constructs a new MultiBinnedParallelCoordinates command class.
 *  @param  argc [in] argumnet count
 *  @param  argv [in] argument values
 */
/*===========================================================================*/
MultiBinnedParallelCoordinates::MultiBinnedParallelCoordinates( int argc, char** argv ):
    Command( argc, argv )
{
}

/*===========================================================================*/
/**
 *  @brief  Executes multi-binned parallel coordinates command.
 *  @return true if the process is done successfully
 */
/*===========================================================================*/
int MultiBinnedParallelCoordinates::exec( void )
{
    Application app( BaseClass::argc(), BaseClass::argv() );

    const std::string name = MultiBinnedParallelCoordinates::CommandName();
    const std::string desc = MultiBinnedParallelCoordinates::CommandDescription();
    const std::string command = std::string( BaseClass::argv(0) ) + " -" + name;
    kvs::CommandLine commandline( BaseClass::argc(), BaseClass::argv(), command );
    commandline.addHelpOption("help");
    commandline.addOption( name, desc + "." );
    commandline.addOption( "verbose", "Verbose output.", 0, false );
    commandline.addOption( "antialiasing", "Enable anti-aliasing. (optional)", 0, false );
    commandline.addOption( "opacity", "Opacity value. (default: 255)", 1, false );
    commandline.addOption( "bgcolor", "Background color. (default: 212 221 229)", 3, false );
    commandline.addOption( "nbins", "Number of bins for table. (default: 10)", 1, false );
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

        if ( kvs::File( filename ).extension() == "mbin" )
        {
            if ( verbose ) std::cout << "Reading " << filename << " ... " << std::flush;
            object = new pcs::MultiBinMapObjectReader( filename );
            if ( verbose ) std::cout << "done." << std::endl;
        }
        else
        {
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

            // Number of bins.
            const size_t ncolumns = table->numberOfColumns();
            kvs::ValueArray<kvs::UInt32> nbins( ncolumns );
            for ( size_t i = 0; i < ncolumns; i++ ) { nbins[i] = 10; }
            if ( commandline.hasOption("nbins") )
            {
                // Set by the specified values.
                std::vector<kvs::UInt32> temp;
                kvs::Tokenizer t( commandline.optionValue<std::string>("nbins"), ",\n\t\0" );
                while ( !t.isLast() )
                {
                    const std::string value = t.token();
                    temp.push_back( kvs::UInt32( std::atoi( value.c_str() ) ) );
                }

                if ( temp.size() == 1 )
                {
                    const kvs::UInt32 value = temp[0];
                    for ( size_t i = 0; i < ncolumns; i++ ) { nbins[i] = value; }
                }
                else if ( temp.size() > 1 )
                {
                    const size_t size = temp.size() > ncolumns ? ncolumns : temp.size();
                    for ( size_t i = 0; i < size; i++ ) { nbins[i] = temp[i]; }
                }
            }

            if ( verbose ) std::cout << "Binning ... " << std::flush;
            object = new pcs::MultiBinMapping( table, nbins );
            delete table;
            if ( verbose ) std::cout << "done." << std::endl;
        }
    }
    else
    {
        if ( verbose ) std::cout << "Creating table object ... " << std::flush;
        const size_t nsamples = 500;
        const size_t naxes = 10;
        kvs::TableObject* table = new kvs::TableObject( util::CreateRandomTable( nsamples, naxes ) );
        if ( verbose ) std::cout << "done." << std::endl;

        // Number of bins.
        const size_t ncolumns = table->numberOfColumns();
        kvs::ValueArray<kvs::UInt32> nbins( ncolumns );
        for ( size_t i = 0; i < ncolumns; i++ ) { nbins[i] = 10; }
        if ( commandline.hasOption("nbins") )
        {
            // Set by the specified values.
            std::vector<kvs::UInt32> temp;
            kvs::Tokenizer t( commandline.optionValue<std::string>("nbins"), ",\n\t\0" );
            while ( !t.isLast() )
            {
                const std::string value = t.token();
                temp.push_back( kvs::UInt32( std::atoi( value.c_str() ) ) );
            }

            if ( temp.size() == 1 )
            {
                const kvs::UInt32 value = temp[0];
                for ( size_t i = 0; i < ncolumns; i++ ) { nbins[i] = value; }
            }
            else if ( temp.size() > 1 )
            {
                const size_t size = temp.size() > ncolumns ? ncolumns : temp.size();
                for ( size_t i = 0; i < size; i++ ) { nbins[i] = temp[i]; }
            }
        }

        if ( verbose ) std::cout << "Binning ... " << std::flush;
        object = new pcs::MultiBinMapping( table, nbins );
        delete table;
        if ( verbose ) std::cout << "done." << std::endl;
    }
    object->setName( object_name );

    if ( verbose )
    {
        const size_t ncolumns = object->numberOfColumns();
        const size_t nrows = object->numberOfRows();
        const kvs::ValueArray<kvs::UInt32>& nbins = object->nbins();
        size_t total_nbins = nbins[0];
        size_t active_bins = object->binList().size();
        for ( size_t i = 1; i < nbins.size(); i++ ) total_nbins *= nbins[i];
        std::cout << "  Number of columns: " << ncolumns << std::endl;
        std::cout << "  Number of nrows: " << nrows << std::endl;
        std::cout << "  Number of sample points: " << object->npoints() << std::endl;
        std::cout << "  Number of bins: " << total_nbins << " (";
        for ( size_t i = 0; i < ncolumns; i++ )
        {
            std::cout << object->label(i) << ":" << nbins[i];
            if ( i < ncolumns - 1 ) std::cout << ",";
        }
        std::cout << ")" << std::endl;
        std::cout << "  Number of active bins: " << active_bins << std::endl;
        std::cout << "  Bin occupancy: " << float( active_bins ) * 100.0 / total_nbins << " %" << std::endl;
    }

    const int top_margin = 40;
    const int bottom_margin = 40;
    const int left_margin = 40;
    const int right_margin = 40;
    const float edge_width = 1.0f;
    const int opacity = commandline.hasOption("opacity") ? commandline.optionValue<int>("opacity") : 255;
    const std::string renderer_name("PCs");
    Renderer* renderer = new Renderer();
    renderer->setName( renderer_name );
    renderer->setTopMargin( top_margin );
    renderer->setBottomMargin( bottom_margin );
    renderer->setLeftMargin( left_margin );
    renderer->setRightMargin( right_margin );
    renderer->setBinEdgeWidth( edge_width );
    renderer->setBinOpacity( opacity );
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
