#include <kvs/GrADS>
#include <kvs/Bounds>
#include <kvs/OrthoSlice>
#include <kvs/StructuredVectorToScalar>
#include <kvs/LineRenderer>
#include <kvs/Streamline>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <util/VortexPoint.h>
#include <util/VortexPointVolume.h>
#include <util/StructuredVolumeImporter.h>
#include <util/CombineVectors.h>
#include <util/CropVolume.h>
#include <util/SeedPoint.h>
#include <util/Timer.h>
#include <util/AdjustVectorScale.h>

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
//    const bool zflip = false;
//    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Rectilinear;
    kvs::StructuredVolumeObject* uvolume = new util::StructuredVolumeImporter( &udata, vindex, tindex, zflip, grid_type );
    kvs::StructuredVolumeObject* vvolume = new util::StructuredVolumeImporter( &vdata, vindex, tindex, zflip, grid_type );
    kvs::StructuredVolumeObject* wvolume = new util::StructuredVolumeImporter( &wdata, vindex, tindex, zflip, grid_type );
//    kvs::Real32* wvalues = (kvs::Real32*)( wvolume->values().data() );
//    for ( size_t i = 0; i < wvolume->nnodes(); i++ ) wvalues[i] *= -1.0f;
    timer.stop();

    timer.start("Combine vectors");
    kvs::StructuredVolumeObject* vector_volume = new util::CombineVectors( uvolume, vvolume, wvolume );
    delete uvolume;
    delete vvolume;
    delete wvolume;
    timer.stop();

    timer.start("Adjust vector scale");
    const kvs::Real32 earth_radius = util::AdjustVectorScale::EarthRadius * 100.0f; // [cm]
    const kvs::Real32 depth_scale = 100.0f; // [m] to [cm]
    kvs::StructuredVolumeObject* volume = new util::AdjustVectorScale( vector_volume, earth_radius, depth_scale );
    delete vector_volume;
    timer.stop();

#if CROP
    timer.start("Crop volume");
//    const kvs::Vector3f min_range( 250, 200, 0 );
//    const kvs::Vector3f max_range( 350, 300, 20 );
    const kvs::Vector3f min_range( 243, 159, 0 );
    const kvs::Vector3f max_range( 360, 243, 30 );
    kvs::StructuredVolumeObject* cropped_volume = new util::CropVolume( volume, min_range, max_range );
    delete volume; volume = cropped_volume;
    timer.stop();
#endif

    timer.start("Extract vortex points");
    kvs::UnstructuredVolumeObject* tensor = new util::VortexPointVolume( volume, ignore_value );
    timer.stop();

    timer.start("Mapping vortex points");
    kvs::PointObject* point = new util::VortexPoint( tensor );
    point->setSize( 3 );
    point->setNormals( kvs::ValueArray<kvs::Real32>(0) ); // delete normal vectors
    point->setColor( kvs::RGBColor( 0, 0, 0 ) );
    delete tensor;
    timer.stop();

    timer.start("Extract seed points");
    const float delta = 0.5;
    kvs::PointObject* seed_point = new util::SeedPoint( point, delta );
    timer.stop();

    timer.start("Extract streamlines");
    kvs::TransferFunction tfunc;
    kvs::Streamline* streamline = new kvs::Streamline();
    streamline->setSeedPoints( seed_point );
    streamline->setIntegrationInterval( 0.1 );
    streamline->setTransferFunction( tfunc );
    streamline->setIntegrationMethod( kvs::Streamline::RungeKutta4th );
    streamline->setVectorLengthThreshold( 0.1 );
    streamline->setIntegrationDirection( kvs::Streamline::BothDirections );
    streamline->setIntegrationTimesThreshold( 400 );
    streamline->setEnableIntegrationTimesCondition( true );
    streamline->setEnableVectorLengthCondition( false );
    kvs::LineObject* line = streamline->exec( volume );
    delete volume;
    delete seed_point;
    timer.stop();

    kvs::glut::Screen screen( &app );
    screen.setTitle( "Streamlines" );
    screen.registerObject( line, new kvs::Bounds() );
    screen.registerObject( line );
    screen.registerObject( point );
    screen.show();

    return app.run();
}
