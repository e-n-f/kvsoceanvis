#include <kvs/GrADS>
#include <kvs/Bounds>
#include <kvs/ExternalFaces>
#include <kvs/KVSMLObjectStructuredVolume>
#include <kvs/StructuredVolumeExporter>
#include <kvs/LineObject>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <util/CropVolume.h>
#include <util/StructuredVolumeImporter.h>
#include <util/Timer.h>

#define OUTPUT 0

using namespace kvsoceanvis;

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    util::Timer timer;
    timer.start("Read data");
    kvs::GrADS data( argv[1] );
    timer.stop();

    timer.start("Import data");
    const size_t vindex = 0;
    const size_t tindex = 0;
    const bool zflip = true;
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
    kvs::StructuredVolumeObject* volume = new util::StructuredVolumeImporter( &data, vindex, tindex, zflip, grid_type );
    timer.stop();

//    kvs::LineObject* bounds = new kvs::Bounds( volume );
    kvs::Bounds* b = new kvs::Bounds();
    kvs::LineObject* bounds = b->outputLineObject( volume );
    delete b;

    timer.start("Crop volume");
    const kvs::Vector3f min_range( 220, 200, 0 );
    const kvs::Vector3f max_range( 350, 300, 50 );
    kvs::StructuredVolumeObject* cropped_volume = new util::CropVolume( volume, min_range, max_range );
    delete volume;
    timer.stop();

#if OUTPUT
    timer.start("Output cropped volume");
    kvs::KVSMLObjectStructuredVolume* kvsml = new kvs::StructuredVolumeExporter<kvs::KVSMLObjectStructuredVolume>( cropped_volume );
    kvsml->setWritingDataType( kvs::KVSMLObjectStructuredVolume::ExternalBinary );
    kvsml->write( "out.kvsml" );
    delete kvsml;
    timer.stop();
#endif

    timer.start("Extract external faces");
    kvs::PolygonObject* object = new kvs::ExternalFaces( cropped_volume );
    object->setMinMaxExternalCoords( min_range, max_range );
    delete cropped_volume;
    timer.stop();

    kvs::glut::Screen screen( &app );
    screen.setTitle( "Crop volume" );
    screen.registerObject( bounds );
    screen.registerObject( object );
    screen.show();

    return app.run();
}
