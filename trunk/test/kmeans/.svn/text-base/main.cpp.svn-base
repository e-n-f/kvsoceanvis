#include <iostream>
#include <fstream>
#include <kvs/Timer>
#include <kvs/TableImporter>
#include <kvs/TableObject>
#include <kvs/ScatterPlotRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/glut/Axis2D>
#include <pcs/KMeansClustering.h>
#include <pcs/FastKMeansClustering.h>
#include <util/CreateRandomTable.h>

using namespace kvsoceanvis;

void KMeansWithRandomSeeding( kvs::glut::Screen& screen, kvs::TableObject* table, size_t nclusters )
{
    pcs::KMeansClustering* clustering = new pcs::KMeansClustering();
    clustering->setSeedingMethod( pcs::KMeansClustering::RandomSeeding );
    clustering->setNumberOfClusters( nclusters );
    clustering->setSeed( 10 );

    kvs::TableObject* object = clustering->exec( table );
    kvs::ScatterPlotRenderer* renderer = new kvs::ScatterPlotRenderer();
    renderer->setPointSize( 20.0 );

    screen.setTitle( "k-means (random seeding)" );
    screen.setBackgroundColor( kvs::RGBColor( 255, 255, 255 ) );
    screen.registerObject( object, new kvs::glut::Axis2D() );
    screen.registerObject( object, renderer );
    screen.show();
}

void KMeansWithSmartSeeding( kvs::glut::Screen& screen, kvs::TableObject* table, size_t nclusters )
{
    pcs::KMeansClustering* clustering = new pcs::KMeansClustering();
    clustering->setSeedingMethod( pcs::KMeansClustering::SmartSeeding );
    clustering->setNumberOfClusters( nclusters );
    clustering->setSeed( 10 );

    kvs::TableObject* object = clustering->exec( table );
    kvs::ScatterPlotRenderer* renderer = new kvs::ScatterPlotRenderer();
    renderer->setPointSize( 20.0 );

    screen.setTitle( "k-means (smart seeding)" );
    screen.setBackgroundColor( kvs::RGBColor( 255, 255, 255 ) );
    screen.registerObject( object, new kvs::glut::Axis2D() );
    screen.registerObject( object, renderer );
    screen.show();
}

void FastKMeansWithRandomSeeding( kvs::glut::Screen& screen, kvs::TableObject* table, size_t nclusters )
{
    pcs::FastKMeansClustering* clustering = new pcs::FastKMeansClustering();
    clustering->setSeedingMethod( pcs::FastKMeansClustering::RandomSeeding );
    clustering->setNumberOfClusters( nclusters );
    clustering->setSeed( 10 );

    kvs::TableObject* object = clustering->exec( table );
    kvs::ScatterPlotRenderer* renderer = new kvs::ScatterPlotRenderer();
    renderer->setPointSize( 20.0 );

    screen.setTitle( "fast k-means (random seeding)" );
    screen.setBackgroundColor( kvs::RGBColor( 255, 255, 255 ) );
    screen.registerObject( object, new kvs::glut::Axis2D() );
    screen.registerObject( object, renderer );
    screen.show();
}

void FastKMeansWithSmartSeeding( kvs::glut::Screen& screen, kvs::TableObject* table, size_t nclusters )
{
    pcs::FastKMeansClustering* clustering = new pcs::FastKMeansClustering();
    clustering->setSeedingMethod( pcs::FastKMeansClustering::SmartSeeding );
    clustering->setNumberOfClusters( nclusters );
    clustering->setSeed( 10 );

    kvs::TableObject* object = clustering->exec( table );
    kvs::ScatterPlotRenderer* renderer = new kvs::ScatterPlotRenderer();
    renderer->setPointSize( 20.0 );

    screen.setTitle( "fast k-means (smart seeding)" );
    screen.setBackgroundColor( kvs::RGBColor( 255, 255, 255 ) );
    screen.registerObject( object, new kvs::glut::Axis2D() );
    screen.registerObject( object, renderer );
    screen.show();
}

int main( int argc, char** argv )
{
    size_t nsamples = 1000;
    size_t dimensions = 2;
    size_t nclusters = 10;
    size_t seed = 0;
    kvs::TableObject* table = new kvs::TableObject( util::CreateRandomTable( nsamples, dimensions, seed ) );

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen_kmeans_random( &app );
    kvs::glut::Screen screen_kmeans_smart( &app );
    kvs::glut::Screen screen_fast_kmeans_random( &app );
    kvs::glut::Screen screen_fast_kmeans_smart( &app );

    KMeansWithRandomSeeding( screen_kmeans_random, table, nclusters );
    KMeansWithSmartSeeding( screen_kmeans_smart, table, nclusters );
    FastKMeansWithRandomSeeding( screen_fast_kmeans_random, table, nclusters );
    FastKMeansWithSmartSeeding( screen_fast_kmeans_smart, table, nclusters );

    delete table;

    return app.run();
}
