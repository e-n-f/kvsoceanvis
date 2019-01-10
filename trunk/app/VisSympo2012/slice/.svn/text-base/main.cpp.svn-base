#include <string>
#include <iostream>
#include <kvs/GrADS>
#include <kvs/OrthoSlice>
#include <kvs/Bounds>
#include <kvs/PolygonRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <util/StructuredVolumeImporter.h>
#include <util/OrthoSlice.h>
#include <util/DegreeToDistance.h>
#include <util/Timer.h>
#include <util/CropVolume.h>

#define CROP 1


int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    kvsoceanvis::util::Timer timer;
    timer.start("Read data");
    kvs::GrADS grads( argv[1] );
    timer.stop();

    timer.start("Import data");
    const size_t vindex = 0;
    const size_t tindex = 0;
    const bool zflip = true;
//    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Rectilinear;
    const kvs::StructuredVolumeObject::GridType grid_type = kvs::StructuredVolumeObject::Uniform;
    kvs::StructuredVolumeObject* volume = new kvsoceanvis::util::StructuredVolumeImporter( &grads, vindex, tindex, zflip, grid_type );
    timer.stop();

/*
    timer.start("Convert degree to distance");
    const float longitude_scale = 91.0f;
    const float latitude_scale = 111.0f;
    const float depth_scale = 0.001f;
    kvs::StructuredVolumeObject* temp = new kvsoceanvis::util::DegreeToDistance( volume, longitude_scale, latitude_scale, depth_scale );
    delete volume; volume = temp;
    timer.stop();
*/
    kvs::StructuredVolumeObject* temp;

    kvs::ValueArray<kvs::Real32> values( volume->numberOfNodes() * 2 );
    kvs::Real32* pvalues = values.data();
    for ( size_t i = 0; i < volume->numberOfNodes(); i++ ) *(pvalues++) = volume->values().at<kvs::Real32>(i);
    for ( size_t i = 0; i < volume->numberOfNodes(); i++ ) *(pvalues++) = volume->values().at<kvs::Real32>(i);

    kvs::Vector3f min_coord( 0.0f, 0.0f, 0.0f );
    kvs::Vector3f max_coord( volume->resolution().x() - 1.0f, volume->resolution().y() - 1.0f, volume->resolution().z() );
    temp = new kvs::StructuredVolumeObject( *volume );
    temp->setResolution( volume->resolution() + kvs::Vector3ui( 0, 0, 1 ) );
    temp->setValues( kvs::AnyValueArray( values ) );
    temp->setMinMaxObjectCoords( min_coord, max_coord );
    temp->setMinMaxExternalCoords( min_coord, max_coord );
    delete volume; volume = temp;


#if CROP
    timer.start("Crop volume");
    const kvs::Vector3f min_range( 250, 200, 0 );
    const kvs::Vector3f max_range( 350, 300, 20 );
    kvs::StructuredVolumeObject* cropped_volume = new kvsoceanvis::util::CropVolume( volume, min_range, max_range );
    delete volume; volume = cropped_volume;
    timer.stop();
#endif

    timer.start("Extract ortho-slice");
//    const float p = ( volume->minObjectCoord().z() + volume->maxObjectCoord().z() ) * 0.5f;
    const float p = 0.0f;
//    const kvsoceanvis::util::OrthoSlice::AlignedAxis a = kvsoceanvis::util::OrthoSlice::ZAxis;
    const kvs::OrthoSlice::AlignedAxis a = kvs::OrthoSlice::ZAxis;
    const kvs::TransferFunction t( 256 );
//    kvs::PolygonObject* object = new kvsoceanvis::util::OrthoSlice( volume, p, a, t );
    kvs::PolygonObject* object = new kvs::OrthoSlice( volume, p, a, t );
    delete volume;
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
