#include <kvs/GrADS>
#include <kvs/Bounds>
#include <kvs/StructuredVectorToScalar>
#include <kvs/StructuredVolumeExporter>
#include <kvs/KVSMLObjectStructuredVolume>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RayCastingRenderer>
#include <util/Vorticity.h>
#include <util/StructuredVolumeImporter.h>
#include <util/CombineVectors.h>
#include <util/CropVolume.h>
#include <util/TransferFunctionEditor.h>
#include <util/Timer.h>

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
    delete uvolume;
    delete vvolume;
    delete wvolume;
    timer.stop();

#if CROP
    timer.start("Crop volume");
    const kvs::Vector3f min_range( 250, 200, 0 );
    const kvs::Vector3f max_range( 350, 300, 20 );
    kvs::StructuredVolumeObject* cropped_volume = new util::CropVolume( volume, min_range, max_range );
    delete volume; volume = cropped_volume;
    timer.stop();
#endif

    timer.start("Calculate vorticity");
    kvs::StructuredVolumeObject* vorticity = new util::Vorticity( volume );
    delete volume;
    timer.stop();

    timer.start("Convert vector to scalar");
    kvs::StructuredVolumeObject* scalar = new kvs::StructuredVectorToScalar( vorticity );
    delete vorticity;
    timer.stop();

    kvs::TransferFunction tfunc( 256 );

    std::string renderer_name("RCR");
    kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
    renderer->setName( renderer_name );
    renderer->setTransferFunction( tfunc );

    kvs::glut::Screen screen( &app );
    screen.setTitle( "Vorticity magnitude" );
    screen.registerObject( scalar, new kvs::Bounds() );
    screen.registerObject( scalar, renderer );
    screen.show();

    util::TransferFunctionEditor<kvs::glsl::RayCastingRenderer> editor( &screen, renderer_name );
    editor.setVolumeObject( scalar );
    editor.setTransferFunction( tfunc );
    editor.show();

    return app.run();
}
