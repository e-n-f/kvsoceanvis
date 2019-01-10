/*****************************************************************************/
/**
 *  @file   LinkedView.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: LinkedView.cpp 342 2013-06-12 04:03:10Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include "LinkedView.h"
#include <kvs/CommandLine>
#include <kvs/File>
#include <kvs/TableObject>
#include <kvs/TableImporter>
#include <kvs/ParallelCoordinatesRenderer>
#include <kvs/PointObject>
#include <kvs/PointRenderer>
#include <kvs/Bounds>
#include <kvs/ColorMap>
#include <kvs/ExternalFaces>
#include <kvs/PolygonObject>
#include <kvs/PolygonRenderer>
#include <kvs/LineObject>
#include <kvs/LineRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/ParallelAxis>

#include <util/CreateRandomTable.h>
#include <util/CreateColorMap.h>
#include <util/Event.h>
#include <util/CsvReader.h>
#include <util/Widget.h>
#include <util/CropVolume.h>

using namespace kvsoceanvis;

typedef kvs::glut::Application Application;
typedef kvs::glut::Screen Screen;
typedef kvs::TableObject Object;
typedef kvs::ParallelCoordinatesRenderer Renderer;
typedef kvs::glut::ParallelAxis Axis;
typedef util::Event::Parameter Parameter;
typedef util::Event::MousePress<Object,Renderer> MousePress;
typedef util::Event::MouseDoubleClick<Object,Renderer> MouseDoubleClick;
typedef util::Event::MouseMove<Object,Renderer> MouseMove;
typedef util::Event::MouseRelease<Object,Renderer> MouseRelease;
typedef util::Event::Paint<Object,Renderer> Paint;
typedef util::Widget::AxisSelector<Object,Renderer> AxisSelector;

template <typename O>
O* GetObject( kvs::ScreenBase* screen_base, const std::string& object_name )
{
    kvs::ObjectManager* manager = NULL;
    kvs::glut::Screen* glut_screen = dynamic_cast<kvs::glut::Screen*>( screen_base );
    if ( glut_screen ) { manager = glut_screen->scene()->objectManager(); }
    if ( !manager ) return NULL;
    else return static_cast<O*>( manager->object( object_name ) );
}

template <typename R>
R* GetRenderer( kvs::ScreenBase* screen_base, const std::string& renderer_name )
{
    kvs::RendererManager* manager = NULL;
    kvs::glut::Screen* glut_screen = dynamic_cast<kvs::glut::Screen*>( screen_base );
    if ( glut_screen ) { manager = glut_screen->scene()->rendererManager(); }
    if ( !manager ) return NULL;
    else return static_cast<R*>( manager->renderer( renderer_name ) );
}

kvs::PointObject* CreatePoints( const Object* object, const size_t active_axis, const kvs::ColorMap& color_map )
{
    kvs::ColorMap cmap( color_map );
    cmap.setRange( object->minValue( active_axis ), object->maxValue( active_axis ) );
    cmap.create();

    const kvs::Vector3f min_value( object->minValue(0), object->minValue(1), object->minValue(2) );
    const kvs::Vector3f max_value( object->maxValue(0), object->maxValue(1), object->maxValue(2) );
    const kvs::Vector3f min_range( object->minRange(0), object->minRange(1), object->minRange(2) );
    const kvs::Vector3f max_range( object->maxRange(0), object->maxRange(1), object->maxRange(2) );

    std::vector<kvs::Real32> coords;
    std::vector<kvs::UInt8> colors;
    for ( size_t i = 0; i < object->numberOfRows(); i++ )
    {
        const kvs::Real32 x = object->column(0).at<kvs::Real32>(i);
        const kvs::Real32 y = object->column(1).at<kvs::Real32>(i);
        const kvs::Real32 z = object->column(2).at<kvs::Real32>(i);

        if ( object->insideRange( i ) )
        {
            const kvs::Real32 v = object->column( active_axis ).at<kvs::Real32>(i);
            const kvs::RGBColor c = cmap.at( v );
            coords.push_back( x );
            coords.push_back( y );
            coords.push_back( z );
            colors.push_back( c.r() );
            colors.push_back( c.g() );
            colors.push_back( c.b() );
        }
    }

    kvs::PointObject* point = new kvs::PointObject();
    point->setCoords( kvs::ValueArray<kvs::Real32>( coords ) );
    point->setColors( kvs::ValueArray<kvs::UInt8>( colors ) );
    point->setSize( 3.0f );
    point->setMinMaxObjectCoords( min_value, max_value );
    point->setMinMaxExternalCoords( min_value, max_value );
    point->setName("Point");

    return point;
}

template <typename Object, typename Renderer>
class UpdatePoints : public kvs::MouseReleaseEventListener
{
private:

    kvs::glut::Screen* m_screen_cartesian;
    std::string m_object_name;
    std::string m_renderer_name;

public:

    UpdatePoints(
        kvs::glut::Screen* screen_cartesian,
        std::string object_name,
        std::string renderer_name )
    {
        m_screen_cartesian = screen_cartesian;
        m_object_name = object_name;
        m_renderer_name = renderer_name;
    }

    void update( kvs::MouseEvent* event )
    {
//        Object* object = static_cast<Object*>( screen()->objectManager()->object( m_object_name ) );
//        Renderer* renderer = static_cast<Renderer*>( screen()->rendererManager()->renderer( m_renderer_name ) );
        Object* object = GetObject<Object>( screen(), m_object_name );
        Renderer* renderer = GetRenderer<Renderer>( screen(), m_renderer_name );

        kvs::ColorMap cmap( 256 );
        cmap.create();

        kvs::PointObject* point = CreatePoints( object, renderer->activeAxis(), cmap );
        m_screen_cartesian->scene()->objectManager()->change( "Point", point );
        m_screen_cartesian->redraw();

        const kvs::Vector3f min_range( object->minRange(0), object->minRange(1), object->minRange(2) );
        const kvs::Vector3f max_range( object->maxRange(0), object->maxRange(1), object->maxRange(2) );
        kvs::PointObject* dummy = new kvs::PointObject();
        dummy->setMinMaxObjectCoords( min_range, max_range );
        kvs::Bounds* bounds = new kvs::Bounds();
        kvs::LineObject* selected_bounds_object = bounds->outputLineObject( dummy );
        delete bounds;
        delete dummy;

//        kvs::LineObject* selected_bounds_object = new kvs::Bounds( min_range, max_range );
        selected_bounds_object->setName("SelectedBounds");
        selected_bounds_object->setColor( kvs::RGBColor( 200, 50, 50 ) );
        selected_bounds_object->setSize( 2.0f );
        m_screen_cartesian->scene()->objectManager()->change( "SelectedBounds", selected_bounds_object );
    }
};

kvs::Vector3ui Resolution( const kvs::TableObject* object )
{
    size_t dimx = 0;
    {
        const kvs::Real64 value0 = object->column(1).at<kvs::Real64>(0);
        for ( size_t i = 0; i < object->numberOfRows(); i++ )
        {
            const kvs::Real64 value = object->column(1).at<kvs::Real64>(i);
            if ( value0 != value ) { dimx = i; break; }
        }
    }

    size_t dimy = 0;
    {
        const kvs::Real64 value0 = object->column(2).at<kvs::Real64>(0);
        for ( size_t i = 0; i < object->numberOfRows(); i++ )
        {
            const kvs::Real64 value = object->column(2).at<kvs::Real64>(i);
            if ( value0 != value ) { dimy = i; break; }
        }
        if ( dimx != 0 ) dimy /= dimx;
    }

    size_t dimz = 0;
    if ( dimx * dimy != 0 ) dimz = object->numberOfRows() / ( dimx * dimy );

    return kvs::Vector3ui( dimx, dimy, dimz );
}

kvs::PolygonObject* CreatePolygon( const Object* object, const size_t active_axis, const kvs::ColorMap& color_map )
{
    kvs::ColorMap cmap( color_map );
    cmap.setRange( object->minValue( active_axis ), object->maxValue( active_axis ) );
    cmap.create();

    const kvs::Vector3ui resolution = Resolution( object );
    const kvs::Vector3f min_value( object->minValue(0), object->minValue(1), object->minValue(2) );
    const kvs::Vector3f max_value( object->maxValue(0), object->maxValue(1), object->maxValue(2) );
    const kvs::Vector3f min_range( object->minRange(0), object->minRange(1), object->minRange(2) );
    const kvs::Vector3f max_range( object->maxRange(0), object->maxRange(1), object->maxRange(2) );

    const kvs::Vector3f min_index(
        ( resolution.x() - 1 ) * ( min_range.x() - min_value.x() ) / ( max_value.x() - min_value.x() ),
        ( resolution.y() - 1 ) * ( min_range.y() - min_value.y() ) / ( max_value.y() - min_value.y() ),
        ( resolution.z() - 1 ) * ( min_range.z() - min_value.z() ) / ( max_value.z() - min_value.z() ) );
    const kvs::Vector3f max_index(
        ( resolution.x() - 1 ) * ( max_range.x() - min_value.x() ) / ( max_value.x() - min_value.x() ),
        ( resolution.y() - 1 ) * ( max_range.y() - min_value.y() ) / ( max_value.y() - min_value.y() ),
        ( resolution.z() - 1 ) * ( max_range.z() - min_value.z() ) / ( max_value.z() - min_value.z() ) );

    kvs::StructuredVolumeObject* volume = new kvs::StructuredVolumeObject();
    volume->setGridType( kvs::StructuredVolumeObject::Uniform );
    volume->setResolution( resolution );
    volume->setVeclen( 1 );
    volume->setValues( object->column( active_axis ) );

    kvs::StructuredVolumeObject* cropped_volume = new util::CropVolume( volume, min_index, max_index );
    cropped_volume->setMinMaxValues( object->minValue( active_axis ), object->maxValue( active_axis ) );
    delete volume;

    kvs::PolygonObject* polygon = new kvs::ExternalFaces( cropped_volume, cmap );
    polygon->setName("Polygon");
    polygon->setMinMaxExternalCoords( min_index, max_index );
    delete cropped_volume;

    return polygon;
}

template <typename Object, typename Renderer>
class UpdatePolygon : public kvs::MouseReleaseEventListener
{
private:

    kvs::glut::Screen* m_screen_cartesian;
    std::string m_object_name;
    std::string m_renderer_name;

public:

    UpdatePolygon(
        kvs::glut::Screen* screen_cartesian,
        std::string object_name,
        std::string renderer_name )
    {
        m_screen_cartesian = screen_cartesian;
        m_object_name = object_name;
        m_renderer_name = renderer_name;
    }

    void update( kvs::MouseEvent* event )
    {
//        Object* object = static_cast<Object*>( screen()->objectManager()->object( m_object_name ) );
//        Renderer* renderer = static_cast<Renderer*>( screen()->rendererManager()->renderer( m_renderer_name ) );
        Object* object = GetObject<Object>( screen(), m_object_name );
        Renderer* renderer = GetRenderer<Renderer>( screen(), m_renderer_name );

        kvs::ColorMap cmap( 256 );
        cmap.create();

        kvs::PolygonObject* polygon = CreatePolygon( object, renderer->activeAxis(), cmap );
        m_screen_cartesian->scene()->objectManager()->change( "Polygon", polygon );
        m_screen_cartesian->redraw();

        const kvs::Vector3f min_range = polygon->minExternalCoord();
        const kvs::Vector3f max_range = polygon->maxExternalCoord();
        kvs::PointObject* dummy = new kvs::PointObject();
        dummy->setMinMaxObjectCoords( min_range, max_range );
        kvs::Bounds* bounds = new kvs::Bounds();
        kvs::LineObject* selected_bounds_object = bounds->outputLineObject( dummy );
        delete bounds;
        delete dummy;

//        kvs::LineObject* selected_bounds_object = new kvs::Bounds( min_range, max_range );
        selected_bounds_object->setName("SelectedBounds");
        selected_bounds_object->setColor( kvs::RGBColor( 200, 50, 50 ) );
        selected_bounds_object->setSize( 2.0f );
        m_screen_cartesian->scene()->objectManager()->change( "SelectedBounds", selected_bounds_object );
    }
};

/*===========================================================================*/
/**
 *  @brief  Constructs a new LinkedView command class.
 *  @param  argc [in] argument count
 *  @param  argv [in] argument values
 */
/*===========================================================================*/
LinkedView::LinkedView( int argc, char** argv ):
    Command( argc, argv )
{
}

/*===========================================================================*/
/**
 *  @brief  Executes parallel coordinates command.
 *  @return true if the process is done successfully
 */
/*===========================================================================*/
int LinkedView::exec( void )
{
    Application app( BaseClass::argc(), BaseClass::argv() );

    const std::string name = LinkedView::CommandName();
    const std::string desc = LinkedView::CommandDescription();
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

    const int top_margin = 50;
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
    renderer->selectAxis( 3 );
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
    MousePress mouse_press( parameter, object_name, renderer_name );
    MouseDoubleClick mouse_double_click( parameter, object_name, renderer_name );
    MouseMove mouse_move( parameter, object_name, renderer_name );
    MouseRelease mouse_release( parameter, object_name, renderer_name );
    Paint paint( parameter, object_name, renderer_name );

    // Screen for parallel coordinates system (pcs)
    const int width = 800;
    const int height = 250;
    Screen screen_pcs( &app );
    screen_pcs.setTitle( desc + ": Parallel coordinates view" );
    screen_pcs.setSize( width, height );
    screen_pcs.registerObject( object, renderer );
    screen_pcs.registerObject( object, axis );
    screen_pcs.addEvent( &mouse_press );
    screen_pcs.addEvent( &mouse_double_click );
    screen_pcs.addEvent( &mouse_move );
    screen_pcs.addEvent( &mouse_release );
    screen_pcs.addEvent( &paint );
    screen_pcs.show();

    if ( commandline.hasOption("bgcolor") )
    {
        const int r = commandline.optionValue<int>("bgcolor",0);
        const int g = commandline.optionValue<int>("bgcolor",1);
        const int b = commandline.optionValue<int>("bgcolor",2);
        screen_pcs.setBackgroundColor( kvs::RGBColor( r, g, b ) );
    }

    AxisSelector selector( &screen_pcs, object_name, renderer_name );
    selector.setLeftMargin( left_margin );
    selector.setRightMargin( right_margin );
    selector.create();
    selector.select( renderer->activeAxis() );
    selector.show();

    // Screen for cartesian coordinates system (ccs)
    Screen screen_ccs( &app );
    screen_ccs.setTitle( desc + ": Cartesian coordinates view" );
    screen_ccs.show();

    if ( commandline.hasValues() )
    {
        kvs::ColorMap cmap( 256 );
        cmap.create();

        kvs::PolygonObject* polygon_object = CreatePolygon( object, renderer->activeAxis(), cmap );
        kvs::PolygonRenderer* polygon_renderer = new kvs::PolygonRenderer();
        screen_ccs.registerObject( polygon_object, polygon_renderer );

        const kvs::Vector3ui resolution = Resolution( object );
        const kvs::Vector3f min_value = kvs::Vector3f( 0.0f, 0.0f, 0.0f );
        const kvs::Vector3f max_value = kvs::Vector3f( resolution.x()-1, resolution.y()-1, resolution.z()-1 );
        kvs::PointObject* dummy = new kvs::PointObject();
        dummy->setMinMaxObjectCoords( min_value, max_value );
        kvs::Bounds* bounds = new kvs::Bounds();
        kvs::LineObject* bounds_object = bounds->outputLineObject( dummy );
        bounds_object->setSize( 2.0f );

        kvs::LineRenderer* bounds_renderer = new kvs::LineRenderer();
        bounds_renderer->enableAntiAliasing();
        screen_ccs.registerObject( bounds_object, bounds_renderer );

        const kvs::Vector3f min_range = polygon_object->minExternalCoord();
        const kvs::Vector3f max_range = polygon_object->maxExternalCoord();
        dummy->setMinMaxObjectCoords( min_range, max_range );
        kvs::LineObject* selected_bounds_object = bounds->outputLineObject( dummy );
        delete bounds;
        delete dummy;

        selected_bounds_object->setName("SelectedBounds");
        selected_bounds_object->setColor( kvs::RGBColor( 200, 50, 50 ) );
        selected_bounds_object->setSize( 2.0f );
        kvs::LineRenderer* selected_bounds_renderer = new kvs::LineRenderer();
        selected_bounds_renderer->enableAntiAliasing();
        screen_ccs.registerObject( selected_bounds_object, selected_bounds_renderer );
    }
    else
    {
        kvs::ColorMap cmap( 256 );
        cmap.create();

        kvs::PointObject* point_object = CreatePoints( object, renderer->activeAxis(), cmap );
        kvs::PointRenderer* point_renderer = new kvs::PointRenderer();
        point_renderer->setName("Point");
        screen_ccs.registerObject( point_object, point_renderer );

        const kvs::Vector3f min_value( object->minValue(0), object->minValue(1), object->minValue(2) );
        const kvs::Vector3f max_value( object->maxValue(0), object->maxValue(1), object->maxValue(2) );
        kvs::PointObject* dummy = new kvs::PointObject();
        dummy->setMinMaxObjectCoords( min_value, max_value );
        kvs::Bounds* bounds = new kvs::Bounds();
        kvs::LineObject* bounds_object = bounds->outputLineObject( dummy );
        bounds_object->setSize( 2.0f );

        kvs::LineRenderer* bounds_renderer = new kvs::LineRenderer();
        bounds_renderer->enableAntiAliasing();
        screen_ccs.registerObject( bounds_object, bounds_renderer );

        const kvs::Vector3f min_range( object->minRange(0), object->minRange(1), object->minRange(2) );
        const kvs::Vector3f max_range( object->maxRange(0), object->maxRange(1), object->maxRange(2) );
        dummy->setMinMaxObjectCoords( min_range, max_range );
        kvs::LineObject* selected_bounds_object = bounds->outputLineObject( dummy );
        delete bounds;
        delete dummy;

        selected_bounds_object->setName("SelectedBounds");
        selected_bounds_object->setColor( kvs::RGBColor( 200, 50, 50 ) );
        selected_bounds_object->setSize( 2.0f );
        kvs::LineRenderer* selected_bounds_renderer = new kvs::LineRenderer();
        selected_bounds_renderer->enableAntiAliasing();
        screen_ccs.registerObject( selected_bounds_object, selected_bounds_renderer );
    }

    UpdatePoints<Object,Renderer> points_updator( &screen_ccs, object_name, renderer_name );
    UpdatePolygon<Object,Renderer> polygon_updator( &screen_ccs, object_name, renderer_name );
    if ( commandline.hasValues() ) { screen_pcs.addEvent( &polygon_updator ); }
    else { screen_pcs.addEvent( &points_updator ); }

    return( app.run() );
}
