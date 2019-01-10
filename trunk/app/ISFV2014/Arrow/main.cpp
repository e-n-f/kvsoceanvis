#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/GrADS>
#include <kvs/Bounds>
#include <kvs/ArrowGlyph>
#include <kvs/TransferFunction>
#include <kvs/RGBFormulae>
#include "Util/Import.h"
#include "Util/Crop.h"
#include "Util/Rescale.h"
#include "Util/Downsize.h"
#include "Util/Arrow.h"


int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    kvs::GrADS* udata = new kvs::GrADS( argv[1] );
    kvs::GrADS* vdata = new kvs::GrADS( argv[2] );
    kvs::GrADS* wdata = new kvs::GrADS( argv[3] );

    const float ignore_value = udata->dataDescriptor().undef().value;

    const size_t tindex = 0;
    const size_t vindex = 0;
    std::cout << "IMPORT FILE" << std::endl;
    std::cout << kvs::Indent(4) << udata->data(tindex).filename() << std::endl;
    std::cout << kvs::Indent(4) << vdata->data(tindex).filename() << std::endl;
    std::cout << kvs::Indent(4) << wdata->data(tindex).filename() << std::endl;

    kvs::StructuredVolumeObject* volume = ISFV2014::Import( udata, vdata, wdata, tindex, vindex );
    volume->print( std::cout << std::endl << "IMPORTED VOLUME" << std::endl, kvs::Indent(4) );

    delete udata;
    delete vdata;
    delete wdata;

    const kvs::Vec3 min_range( 141, 35, -1000 ); // in (deg, deg, meter)
    const kvs::Vec3 max_range( 147, 43, 0 ); // in (deg, deg, meter)
    kvs::StructuredVolumeObject* cropped_volume = ISFV2014::Crop( volume, min_range, max_range );

    const kvs::Vec3 coord_scale( 1, 1, 200 );
    const kvs::Vec3 value_scale( 1, 1, 1 );
    ISFV2014::Rescale::Coords( cropped_volume, coord_scale );
    ISFV2014::Rescale::Values( cropped_volume, value_scale );

    cropped_volume->print( std::cout << std::endl << "CROPPED VOLUME" << std::endl, kvs::Indent(4) );
    delete volume;

    const kvs::Vec3ui stride( 3, 3, 3 );
    kvs::StructuredVolumeObject* downsized_volume = ISFV2014::Downsize( cropped_volume, stride );
    downsized_volume->print( std::cout << std::endl << "DOWNSIZED VOLUME" << std::endl, kvs::Indent(4) );
    delete cropped_volume;

    ISFV2014::ArrowGlyph* arrow = new ISFV2014::ArrowGlyph();
    const kvs::TransferFunction transfer_function( kvs::RGBFormulae::Jet( 256 ) );
    //const ISFV2014::ArrowGlyph::ArrowType type = ISFV2014::ArrowGlyph::LineArrow;
    const ISFV2014::ArrowGlyph::ArrowType type = ISFV2014::ArrowGlyph::TubeArrow;
    const ISFV2014::ArrowGlyph::SizeMode mode = ISFV2014::ArrowGlyph::SizeByDefault;
    //const ISFV2014::ArrowGlyph::SizeMode mode = ISFV2014::ArrowGlyph::SizeByMagnitude;
    arrow->setTransferFunction( transfer_function );
    arrow->setIgnoreValue( ignore_value );
    arrow->setType( type );
    arrow->setSizeMode( mode );
    arrow->setScale( 45 );

    kvs::Bounds* bounds = new kvs::Bounds();
    bounds->setLineWidth( 2.0f );

    kvs::glut::Screen screen( &app );
    screen.setTitle( "Arrow" );
    screen.registerObject( downsized_volume, arrow );
    screen.registerObject( downsized_volume, bounds );
    screen.show();

    return app.run();
}
