#include <kvs/GrADS>
#include <kvs/Bounds>
#include <kvs/OrthoSlice>
#include <kvs/StructuredVectorToScalar>
#include <kvs/PolygonRenderer>
#include <kvs/Streamline>
#include <kvs/RayCastingRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/RayCastingRenderer>
#include <util/VortexPoint.h>
#include <util/VortexPointVolume.h>
#include <util/StructuredVolumeImporter.h>
#include <util/CombineVectors.h>
#include <util/CropVolume.h>
#include <util/SeedPoint.h>
#include <util/Vorticity.h>
#include <util/Timer.h>
#include <util/TransferFunctionEditor.h>

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
    delete point;
    timer.stop();

    timer.start("Extract streamlines");
    kvs::Streamline* streamline = new kvs::Streamline();
    streamline->setSeedPoints( seed_point );
    streamline->setIntegrationInterval( 0.2 );
    streamline->setIntegrationMethod( kvs::Streamline::RungeKutta2nd );
    streamline->setVectorLengthThreshold( 0.2 );
    streamline->setIntegrationDirection( kvs::Streamline::BothDirections );
    streamline->setIntegrationTimesThreshold( 100 );
    streamline->setEnableIntegrationTimesCondition( true );
    streamline->setEnableVectorLengthCondition( false );
    kvs::LineObject* line = streamline->exec( volume );
    delete seed_point;
    timer.stop();

    kvs::StructuredVolumeObject* scalar = NULL;
    float min_value = kvs::Value<float>::Max();
    float max_value = kvs::Value<float>::Min();
    if ( argc > 4 )
    {
        delete volume;

        timer.start("Read 4th data");
        kvs::GrADS data( argv[4] );
        scalar = new util::StructuredVolumeImporter( &data, vindex, tindex, zflip, grid_type );
        timer.stop();

#if CROP
        timer.start("Crop 4th data");
        kvs::StructuredVolumeObject* cropped_volume = new util::CropVolume( scalar, min_range, max_range );
        delete scalar; scalar = cropped_volume;
        timer.stop();

        const float ignore_value = data.dataDescriptor().undef().value;
        const float* values = static_cast<const float*>(scalar->values().data());
        for ( size_t i = 0; i < scalar->values().size(); i++ )
        {
            const float value = *(values++);

            if ( value != ignore_value )
            {
                min_value = kvs::Math::Min( min_value, value );
                max_value = kvs::Math::Max( max_value, value );
            }
        }

        scalar->setMinMaxValues( min_value, max_value );
#endif
    }
    else
    {
        timer.start("Calculate vorticity");
        kvs::StructuredVolumeObject* vorticity = new util::Vorticity( volume );
        delete volume;
        timer.stop();

        timer.start("Calculate magnitude of vorticity");
        scalar = new kvs::StructuredVectorToScalar( vorticity );
        delete vorticity;
        timer.stop();
    }

    kvs::TransferFunction tfunc( 256 );
    tfunc.setRange( min_value, max_value );

    std::string renderer_name("RCR");
    kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
    renderer->setName( renderer_name );
    renderer->setTransferFunction( tfunc );

    kvs::glut::Screen screen( &app );
    screen.setTitle( "Streamlines with volume" );
    screen.registerObject( line, new kvs::Bounds() );
    screen.registerObject( line );
    screen.registerObject( scalar, renderer );
    screen.show();

    util::TransferFunctionEditor<kvs::glsl::RayCastingRenderer> editor( &screen, renderer_name );
    editor.setVolumeObject( scalar );
    editor.setTransferFunction( tfunc );
    editor.show();

    return app.run();
}
