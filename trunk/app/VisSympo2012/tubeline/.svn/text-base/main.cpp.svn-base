#include <kvs/GrADS>
#include <kvs/Bounds>
#include <kvs/OrthoSlice>
#include <kvs/StructuredVectorToScalar>
#include <kvs/PolygonRenderer>
#include <kvs/Streamline>
#include <kvs/Tubeline>
#include <kvs/TransferFunction>
#include <kvs/RayCastingRenderer>
#include <kvs/PolygonRenderer>
#include <kvs/RayCastingRenderer>
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
#include <util/TransferFunctionEditor.h>
#include "Streamline.h"

#include <kvs/RGBFormulae>

#define CROP 1
#define TUBE 1


int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    kvsoceanvis::util::Timer timer;
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
    const bool wflip = false;
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Rectilinear;
    kvs::StructuredVolumeObject* uvolume = new kvsoceanvis::util::StructuredVolumeImporter( &udata, vindex, tindex, zflip, grid_type );
    kvs::StructuredVolumeObject* vvolume = new kvsoceanvis::util::StructuredVolumeImporter( &vdata, vindex, tindex, zflip, grid_type );
    kvs::StructuredVolumeObject* wvolume = new kvsoceanvis::util::StructuredVolumeImporter( &wdata, vindex, tindex, zflip, grid_type );
    if ( wflip )
    {
        kvs::Real32* wvalues = (kvs::Real32*)( wvolume->values().data() );
        for ( size_t i = 0; i < wvolume->numberOfNodes(); i++ ) wvalues[i] *= -1.0f;
    }
    timer.stop();

    timer.start("Combine vectors");
    kvs::StructuredVolumeObject* vector_volume = new kvsoceanvis::util::CombineVectors( uvolume, vvolume, wvolume );
    delete uvolume;
    delete vvolume;
    delete wvolume;
    timer.stop();

    timer.start("Adjust vector scale");
//    const kvs::Real32 earth_radius = kvsoceanvis::util::AdjustVectorScale::EarthRadius * 100.0f; // [cm]
    const kvs::Real32 earth_radius = kvsoceanvis::util::AdjustVectorScale::EarthRadius; // [cm]
//    const kvs::Real32 depth_scale = 100.0f; // [m] to [cm]
    const kvs::Real32 depth_scale = 1.0f; // [m] to [cm]
    kvs::StructuredVolumeObject* volume = new kvsoceanvis::util::AdjustVectorScale( vector_volume, earth_radius, depth_scale );
    delete vector_volume;
    timer.stop();


#if CROP
    timer.start("Crop volume");
//    const kvs::Vector3f min_range( 250, 200, 0 );
//    const kvs::Vector3f max_range( 350, 300, 20 );
    const kvs::Vector3f min_range( 250, 150, 0 );
    const kvs::Vector3f max_range( 350, 250, 20 );
    kvs::StructuredVolumeObject* cropped_volume = new kvsoceanvis::util::CropVolume( volume, min_range, max_range );
    delete volume; volume = cropped_volume;
    timer.stop();
#endif

    timer.start("Extract vortex points");
    kvs::UnstructuredVolumeObject* tensor = new kvsoceanvis::util::VortexPointVolume( volume, ignore_value );
    timer.stop();

    timer.start("Mapping vortex points");
    kvs::PointObject* point = new kvsoceanvis::util::VortexPoint( tensor );
    point->setSize( 3 );
    point->setNormals( kvs::ValueArray<kvs::Real32>(0) ); // delete normal vectors
    point->setColor( kvs::RGBColor( 0, 0, 0 ) );
    delete tensor;
    timer.stop();

    timer.start("Extract seed points");
    const float delta = 0.5;
    kvs::PointObject* seed_point = new kvsoceanvis::util::SeedPoint( point, delta );
    timer.stop();

    timer.start("Extract streamlines");
    kvs::TransferFunction tfunc;
#if 1
    kvs::Streamline* streamline = new kvs::Streamline();
    streamline->setSeedPoints( seed_point );
    streamline->setTransferFunction( tfunc );
    streamline->setIntegrationMethod( kvs::Streamline::RungeKutta4th );
    streamline->setIntegrationDirection( kvs::Streamline::BothDirections );
    streamline->setIntegrationInterval( 0.03 );
    streamline->setIntegrationTimesThreshold( 400 );
    streamline->setVectorLengthThreshold( 0.1 );
    streamline->setEnableIntegrationTimesCondition( true );
    streamline->setEnableVectorLengthCondition( false );
#else
    test::Streamline* streamline = new test::Streamline();
    streamline->setVortexes( seed_point );
    streamline->setIntegrationInterval( 0.1 );
    streamline->setTransferFunction( tfunc );
    streamline->setIntegrationMethod( test::Streamline::RungeKutta4th );
    streamline->setVectorLengthThreshold( 0.1 );
    //streamline->setEnableIntegrationTimesCondition(false);
    streamline->setIntegrationTimesThreshold( 400 );
    streamline->setEnableVectorLengthCondition( true );
    streamline->setIgnoreValue( ignore_value );
#endif
    kvs::LineObject* line = streamline->exec( volume );
    delete volume;
    delete seed_point;
    timer.stop();

#if TUBE
    timer.start("Extract tubelines");
//    line->setSize( 0.35 );
    line->setSize( 0.4 );
    kvs::PolygonObject* tube = new kvs::Tubeline( line, 20 );
    delete line;
    timer.stop();
#endif


    kvs::StructuredVolumeObject* scalar = NULL;
    float min_value = kvs::Value<float>::Max();
    float max_value = kvs::Value<float>::Min();
    if ( argc > 4 )
    {
//        delete volume;

        timer.start("Read 4th data");
        kvs::GrADS data( argv[4] );
        scalar = new kvsoceanvis::util::StructuredVolumeImporter( &data, vindex, tindex, zflip, kvs::StructuredVolumeObject::Uniform );
        timer.stop();

#if CROP
        timer.start("Crop 4th data");
        kvs::StructuredVolumeObject* cropped_volume = new kvsoceanvis::util::CropVolume( scalar, min_range, max_range );
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


    kvs::glut::Screen screen( &app );
    screen.setTitle( "Tubeline" );
#if TUBE
    kvs::ValueArray<kvs::Real32> normals = tube->normals();
    for ( size_t i = 0; i < tube->normals().size(); i++ ) normals[i] *= -1.0f;
    kvs::glsl::PolygonRenderer* renderer = new kvs::glsl::PolygonRenderer();
//    renderer->setShader( kvs::Shader::Lambert() );
    renderer->setShader( kvs::Shader::BlinnPhong() );

    tube->setColor( kvs::RGBColor( 200, 200, 100 ) );

    screen.registerObject( tube, new kvs::Bounds() );
    screen.registerObject( tube, renderer );
#else
    screen.registerObject( line, new kvs::Bounds() );
    screen.registerObject( line );
#endif
    screen.registerObject( point );
    screen.show();

    screen.setBackgroundColor( kvs::RGBColor( 255, 255, 255 ) );

    kvs::TransferFunction voltfunc( 256 );
    voltfunc.setRange( min_value, max_value );

    kvs::ColorMap cmap = kvs::RGBFormulae::Rainbow( 256 );
    cmap.setRange( min_value, max_value );
    voltfunc.setColorMap( cmap );

    std::string renderer_name("RCR");
    kvs::glsl::RayCastingRenderer* volren = new kvs::glsl::RayCastingRenderer();
//    kvs::RayCastingRenderer* volren = new kvs::RayCastingRenderer();
    volren->setName( renderer_name );
    volren->setTransferFunction( voltfunc );
    volren->setShader( kvs::Shader::Lambert( 0.5, 0.8 ) );
    screen.registerObject( scalar, volren );

    kvsoceanvis::util::TransferFunctionEditor<kvs::glsl::RayCastingRenderer> editor( &screen, renderer_name );
//    kvsoceanvis::util::TransferFunctionEditor<kvs::RayCastingRenderer> editor( &screen, renderer_name );
    editor.setVolumeObject( scalar );
    editor.setTransferFunction( voltfunc );
    editor.show();

    return app.run();
}
