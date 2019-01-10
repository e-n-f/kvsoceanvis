#include <string>
#include <iostream>
#include <kvs/GrADS>
#include <kvs/Isosurface>
#include <kvs/Bounds>
#include <kvs/PolygonRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <util/StructuredVolumeImporter.h>
#include <util/Isosurface.h>
#include <util/DegreeToDistance.h>
#include <util/Timer.h>

using namespace kvsoceanvis;

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    util::Timer timer;
    timer.start("Read data");
    kvs::GrADS grads( argv[1] );
    timer.stop();

    timer.start("Import data");
    const size_t vindex = 0;
    const size_t tindex = 0;
    const bool zflip = true;
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Rectilinear;
    kvs::StructuredVolumeObject* volume1 = new util::StructuredVolumeImporter( &grads, vindex, tindex, zflip, grid_type );
    timer.stop();

    timer.start("Convert degree to distance");
    const float longitude_scale = 91.0f;
    const float latitude_scale = 111.0f;
    const float depth_scale = 0.001f;
    kvs::StructuredVolumeObject* volume2 = new util::DegreeToDistance( volume1, longitude_scale, latitude_scale, depth_scale );
    delete volume1;
    timer.stop();

    timer.start("Extract isosuraces");
    const double i = ( volume2->maxValue() + volume2->minValue() ) * 0.5;
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::PolygonNormal;
    const bool d = false;
    const kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object = new util::Isosurface( volume2, i, n, d, t );
    delete volume2;
    timer.stop();

    kvs::glut::Screen screen( &app );
    screen.setTitle( "Isosurface" );
    screen.registerObject( object, new kvs::Bounds() );
    screen.registerObject( object );
    screen.show();

    return app.run();
}
