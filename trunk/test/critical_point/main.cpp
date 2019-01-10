#include <kvs/GrADS>
#include <kvs/Bounds>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <util/CriticalPoint.h>
#include <util/CriticalPointVolume.h>
#include <util/StructuredVolumeImporter.h>
#include <util/CombineVectors.h>
#include <util/Timer.h>
#include <util/CropVolume.h>

#define CROP 1

using namespace kvsoceanvis;

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    util::Timer timer;
    timer.start("Read data");
    kvs::GrADS udata( argv[1] );
    kvs::GrADS vdata( argv[2] );
    kvs::GrADS wdata( argv[3] );
    timer.stop();

    const float ignore_value = udata.dataDescriptor().undef().value;

    timer.start("Import data");
    const size_t vindex = 0;
    const size_t tindex = 0;
    const bool zflip = true;
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
    kvs::StructuredVolumeObject* uvolume = new util::StructuredVolumeImporter( &udata, vindex, tindex, zflip, grid_type );
    kvs::StructuredVolumeObject* vvolume = new util::StructuredVolumeImporter( &vdata, vindex, tindex, zflip, grid_type );
    kvs::StructuredVolumeObject* wvolume = new util::StructuredVolumeImporter( &wdata, vindex, tindex, zflip, grid_type );
    timer.stop();

    timer.start("Combine vectors");
    kvs::StructuredVolumeObject* volume = new util::CombineVectors( uvolume, vvolume, wvolume );
    timer.stop();
    delete uvolume;
    delete vvolume;
    delete wvolume;

#if CROP
    timer.start("Crop volume");
    const kvs::Vector3f min_range( 250, 200, 0 );
    const kvs::Vector3f max_range( 350, 300, 20 );
    kvs::StructuredVolumeObject* cropped_volume = new util::CropVolume( volume, min_range, max_range );
    timer.stop();
    delete volume;
    volume = cropped_volume;
#endif

    timer.start("Extract critical points");
    kvs::UnstructuredVolumeObject* tensor = new util::CriticalPointVolume( volume, ignore_value );
    timer.stop();
    delete volume;

    timer.start("Mapping critical points");
    kvs::PointObject* point = new util::CriticalPoint( tensor );
    timer.stop();
    delete tensor;

    kvs::glut::Screen screen( &app );
    screen.setTitle( "Critical point" );
    screen.registerObject( point );
    screen.registerObject( point, new kvs::Bounds() );
    screen.show();

    return app.run();
}
