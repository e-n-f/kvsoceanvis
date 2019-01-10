#include <iostream>
#include <kvs/Gis>
#include <kvs/PolygonObject>
#include <kvs/PolygonRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <util/PolygonImporter.h>
#include <util/Timer.h>

#define JAPAN_AREA 0

using namespace kvsoceanvis;

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    util::Timer timer;
    timer.start("Read data");
#if JAPAN_AREA
    kvs::gis::Area japan( 25.7167, 46.2749, 125.0000, 150.0000 );
    kvs::Gis* gis = new kvs::Gis( argv[1], japan );
#else
    kvs::Gis* gis = new kvs::Gis( argv[1] );
#endif
    timer.stop();

    std::cout << "min. latitude: " << gis->area().minLatitude() << std::endl;
    std::cout << "max. latitude: " << gis->area().maxLatitude() << std::endl;
    std::cout << "min. longitude: " << gis->area().minLongitude() << std::endl;
    std::cout << "max. longitude: " << gis->area().maxLongitude() << std::endl;
    std::cout << "min. height: " << gis->area().minHeight() << std::endl;
    std::cout << "max. height: " << gis->area().maxHeight() << std::endl;
    std::cout << std::endl;
    std::cout << "min. render latitude: " << gis->renderArea().minLatitude() << std::endl;
    std::cout << "max. render latitude: " << gis->renderArea().maxLatitude() << std::endl;
    std::cout << "min. render longitude: " << gis->renderArea().minLongitude() << std::endl;
    std::cout << "max. render longitude: " << gis->renderArea().maxLongitude() << std::endl;
    std::cout << "min. render height: " << gis->renderArea().minHeight() << std::endl;
    std::cout << "max. render height: " << gis->renderArea().maxHeight() << std::endl;
    std::cout << "num. of meshes: " << gis->meshes().size() << std::endl;
    std::cout << std::endl;

    const size_t nmeshes = gis->meshes().size();
    for ( size_t i = 0; i < nmeshes; i++ )
    {
        std::cout << "mesh[" << i << "]" << std::endl;
        std::cout << "row: " << gis->mesh(i)->row() << std::endl;
        std::cout << "column: " << gis->mesh(i)->column() << std::endl;
        std::cout << "latitude dimension: " << gis->mesh(i)->latitudeDimension() << std::endl;
        std::cout << "longitude dimension: " << gis->mesh(i)->longitudeDimension() << std::endl;
        std::cout << std::endl;
    }

    timer.start("Import data");
    kvs::PolygonObject* object = new util::PolygonImporter( gis );
    delete gis;
    timer.stop();

    kvs::PolygonRenderer* renderer = new kvs::PolygonRenderer();

    kvs::glut::Screen screen( &app );
    screen.registerObject( object, renderer );
    screen.show();

    return app.run();
}
