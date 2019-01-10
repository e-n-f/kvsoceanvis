#include <string>
#include <iostream>
#include <kvs/GrADS>
#include <kvs/Isosurface>
#include <kvs/Bounds>
#include <kvs/PolygonRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <util/StructuredVolumeImporter.h>
#include <util/OrthoSlice.h>
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

    timer.start("Extract ortho-slice");
    const float p = ( volume2->minObjectCoord().z() + volume2->maxObjectCoord().z() ) * 0.5f;
    const util::OrthoSlice::AlignedAxis a = util::OrthoSlice::ZAxis;
    const kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object = new util::OrthoSlice( volume2, p, a, t );
    delete volume2;
    timer.stop();

    kvs::PolygonRenderer* renderer = new kvs::PolygonRenderer();
    renderer->disableShading();

    kvs::glut::Screen screen( &app );
    screen.setTitle( "Ortho slice" );
    screen.registerObject( object, renderer );
    screen.registerObject( object, new kvs::Bounds() );
    screen.show();

    return app.run();
}
