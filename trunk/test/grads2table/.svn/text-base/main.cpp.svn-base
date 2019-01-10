/*****************************************************************************/
/**
 *  @file   main.cpp
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
#include <string>
#include <iostream>
#include <kvs/GrADS>
#include <kvs/File>
#include <kvs/StructuredVolumeObject>
#include <kvs/TableObject>
#include <kvs/ParallelCoordinatesRenderer>
#include <kvs/KVSMLObjectTable>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/ParallelAxis>
#include <util/Timer.h>
#include <util/CoordExtractor.h>
#include <util/StructuredVolumeImporter.h>
#include <util/CropVolume.h>
#include <util/Event.h>
#include <util/TableObjectWriter.h>

using namespace kvsoceanvis;

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    util::Timer timer;
    timer.start("Read data");
    kvs::GrADS udata( argv[1] );
    kvs::GrADS vdata( argv[2] );
    kvs::GrADS wdata( argv[3] );
    kvs::GrADS tdata( argv[4] );
    timer.stop();

    timer.start("Extract coords");
    const kvs::Vector3f min_range( 250, 200, 0 );
    const kvs::Vector3f max_range( 350, 300, 20 );
    util::CoordExtractor extractor( &udata, min_range, max_range );
    kvs::ValueArray<kvs::Real32> xcoords = extractor.alignedCoordX();
    kvs::ValueArray<kvs::Real32> ycoords = extractor.alignedCoordY();
    kvs::ValueArray<kvs::Real32> zcoords = extractor.alignedCoordZ();
    timer.stop();

    timer.start("Import data");
    const size_t vindex = 0;
    const size_t tindex = 0;
    const bool zflip = false;
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
    kvs::StructuredVolumeObject* wvolume = new util::StructuredVolumeImporter( &wdata, vindex, tindex, zflip, grid_type );
    kvs::StructuredVolumeObject* tvolume = new util::StructuredVolumeImporter( &tdata, vindex, tindex, zflip, grid_type );
    timer.stop();

    timer.start("Crop volume");
    kvs::StructuredVolumeObject* cropped_wvolume = new util::CropVolume( wvolume, min_range, max_range );
    kvs::StructuredVolumeObject* cropped_tvolume = new util::CropVolume( tvolume, min_range, max_range );
    delete wvolume; wvolume = cropped_wvolume;
    delete tvolume; tvolume = cropped_tvolume;
    timer.stop();

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

    std::string object_name("Object");
    Object* object = new Object();
    object->setName( object_name );
    object->addColumn( xcoords, "lon" );
    object->addColumn( ycoords, "lat" );
    object->addColumn( zcoords, "dep" );
    object->addColumn( tvolume->values(), "t" );
    object->addColumn( wvolume->values(), "w" );
    delete wvolume;
    delete tvolume;

    std::string renderer_name("renderer");
    Renderer* renderer = new Renderer();
    renderer->setName( renderer_name );
    renderer->enableAntiAliasing();
    renderer->setLeftMargin( 30 );
    renderer->setRightMargin( 30 );
    renderer->setBottomMargin( 30 );

    Axis* axis = new Axis();
    axis->setLeftMargin( 30 );
    axis->setRightMargin( 30 );
    axis->setBottomMargin( 30 );

    Parameter parameter;
    KeyPress key_press( parameter, object_name, renderer_name );
    MousePress mouse_press( parameter, object_name, renderer_name );
    MouseDoubleClick mouse_double_click( parameter, object_name, renderer_name );
    MouseMove mouse_move( parameter, object_name, renderer_name );
    MouseRelease mouse_release( parameter, object_name, renderer_name );
    Paint paint( parameter, object_name, renderer_name );

    kvs::glut::Screen screen( &app );
    screen.setTitle("GrADS2Table");
    screen.setSize( 800, 450 );
    screen.registerObject( object, renderer );
    screen.registerObject( object, axis );
    screen.addEvent( &key_press );
    screen.addEvent( &mouse_press );
    screen.addEvent( &mouse_double_click );
    screen.addEvent( &mouse_move );
    screen.addEvent( &mouse_release );
    screen.addEvent( &paint );
    screen.show();

    timer.start("Write data");
    util::TableObjectWriter writer( object );
    writer.write( argc > 5 ? argv[5] : "output.kvsml" );
    timer.stop();

    return app.run();
}
