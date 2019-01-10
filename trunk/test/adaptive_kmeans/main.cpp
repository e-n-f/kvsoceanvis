#include <kvs/MersenneTwister>
#include <kvs/TableObject>
#include <kvs/ScatterPlotRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/Axis2D>
#include <pcs/AdaptiveKMeansClustering.h>

using namespace kvsoceanvis;


kvs::TableObject CreateTable( const size_t nsamples )
{
    kvs::Real32 xmin = 2.0f;
    kvs::Real32 xmax = 6.0f;
    kvs::Real32 ymin = 2.0f;
    kvs::Real32 ymax = 6.0f;

    kvs::ValueArray<kvs::Real32> x( nsamples );
    kvs::ValueArray<kvs::Real32> y( nsamples );

    kvs::MersenneTwister R;
    for ( size_t i = 0; i < nsamples / 4; i++ )
    {
        x[i] = -( R() * ( xmax - xmin ) + xmin );
        y[i] = +( R() * ( ymax - ymin ) + ymin );
    }

    for ( size_t i = nsamples / 4; i < nsamples / 4 * 2; i++ )
    {
        x[i] = -( R() * ( xmax - xmin ) + xmin );
        y[i] = -( R() * ( ymax - ymin ) + ymin );
    }

    for ( size_t i = nsamples / 4 * 2; i < nsamples / 4 * 3; i++ )
    {
        x[i] = +( R() * ( xmax - xmin ) + xmin );
        y[i] = -( R() * ( ymax - ymin ) + ymin );
    }

    for ( size_t i = nsamples / 4 * 3; i < nsamples; i++ )
    {
        x[i] = +( R() * ( xmax - xmin ) + xmin );
        y[i] = +( R() * ( ymax - ymin ) + ymin );
    }

    kvs::TableObject table;
    table.addColumn( x );
    table.addColumn( y );

    return table;
}

int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );

    const size_t nsamples = 100;
    kvs::TableObject* table = new kvs::TableObject( CreateTable( nsamples ) );
    kvs::TableObject* object = new pcs::AdaptiveKMeansClustering( table );
    delete table;

    kvs::ScatterPlotRenderer* renderer = new kvs::ScatterPlotRenderer();
    renderer->setPointSize( 10.0 );

    kvs::glut::Screen screen( &app );
    screen.registerObject( object, new kvs::glut::Axis2D() );
    screen.registerObject( object, renderer );
    screen.show();

    return app.run();
}
