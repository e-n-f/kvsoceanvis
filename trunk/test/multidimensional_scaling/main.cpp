#include <iostream>
#include <fstream>
#include <string>
#include <kvs/Timer>
#include <kvs/TableImporter>
#include <kvs/ScatterPlotRenderer>
#include <kvs/glut/Axis2D>
#include <kvs/PointRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <pcs/MultiDimensionalScaling.h>
#include <util/Timer.h>

using namespace kvsoceanvis;

template <typename ValueType>
kvs::Matrix<ValueType> DistanceMatrix( void )
{
    const ValueType M[100] = {
        0,587,1212,701,1936,604,748,2139,2182,543,
        587,0,920,940,1745,1188,713,1858,1737,597,
        1212,920,0,879,831,1726,1631,949,1021,1494,
        701,940,879,0,1374,968,1420,1645,1891,1220,
        1936,1745,831,1374,0,2339,2451,347,959,2300,
        604,1188,1726,968,2339,0,1092,2594,2734,923,
        748,713,1631,1420,2451,1092,0,2571,2408,205,
        2139,1858,949,1645,347,2594,2571,0,678,2442,
        2182,1737,1021,1891,959,2734,2408,678,0,2329,
        543,597,1494,1220,2300,923,205,2442,2329,0
    };

    return kvs::Matrix<ValueType>( 10, 10, M );
}

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );

    util::Timer timer;
    kvs::TableObject* object = NULL;
    if ( argc > 1 )
    {
        timer.start("Import data");
        kvs::TableObject* table = new kvs::TableImporter( argv[1] );
        timer.stop();

        timer.start("Multidimensional scaling");
        object = new pcs::MultiDimensionalScaling( table );
        delete table;
        timer.stop();
    }
    else
    {
        timer.start("Create distance matrix");
        kvs::Matrix<kvs::Real32> D = DistanceMatrix<kvs::Real32>();
        timer.stop();

        timer.start("Multidimensional scaling");
        object = new pcs::MultiDimensionalScaling( D );
        timer.stop();
    }

    kvs::ScatterPlotRenderer* renderer = new kvs::ScatterPlotRenderer();
    kvs::glut::Axis2D* axis = new kvs::glut::Axis2D();
    renderer->setPointSize( 10 );

    screen.registerObject( object, renderer );
    screen.registerObject( object, axis );
    screen.show();

    return app.run();
}
