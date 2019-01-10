#include <kvs/GrADS>
#include <kvs/ExternalFaces>
#include <kvs/TransferFunction>
#include <kvs/RGBFormulae>
#include <kvs/LineObject>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include "Util/Import.h"
#include "Util/Crop.h"
#include "Util/Rescale.h"


int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    kvs::GrADS* data = new kvs::GrADS( argv[1] );

    const size_t tindex = 0;
    const size_t vindex = 0;
    std::cout << "IMPORT FILE" << std::endl;
    std::cout << kvs::Indent(4) << data->data(tindex).filename() << std::endl;

    kvs::StructuredVolumeObject* volume = ISFV2014::Import( data, tindex, vindex );
    volume->print( std::cout << std::endl << "ORIGINAL VOLUME" << std::endl, kvs::Indent(4) );
    delete data;

    const kvs::Vec3 min_range( 141, 33, -1000 ); // in (deg, deg, meter)
    const kvs::Vec3 max_range( 147, 43, 0 ); // in (deg, deg, meter)
    kvs::StructuredVolumeObject* cropped_volume = ISFV2014::Crop( volume, min_range, max_range );
    cropped_volume->print( std::cout << std::endl << "CROPPED VOLUME (deg)" << std::endl, kvs::Indent(4) );
    delete volume;

    const kvs::Vec3 coord_scale( 1, 1, 200 );
    ISFV2014::Rescale::Coords( cropped_volume, coord_scale );

//    const kvs::TransferFunction transfer_function( kvs::RGBFormulae::Jet( 256 ) );
    const kvs::TransferFunction transfer_function( 256 );
    kvs::PolygonObject* object = new kvs::ExternalFaces( cropped_volume, transfer_function );
    object->print( std::cout << std::endl << "EXTERNAL FACES" << std::endl, kvs::Indent(4) );
    delete cropped_volume;

    kvs::LineObject* edges = new kvs::LineObject( *object );
    edges->setColor( kvs::RGBColor::Black() );
    edges->setSize( 2.0f );

    kvs::glut::Screen screen( &app );
    screen.setSize( 800, 600 );
    screen.setTitle( "Crop volume" );
    screen.registerObject( object );
    screen.registerObject( edges );
    screen.show();

    return app.run();
}
