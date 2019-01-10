
#include <iostream>
#include <fstream>
#include <string>
#include <kvs/Timer>
#include <kvs/TableImporter>
#include <kvs/ScatterPlotRenderer>
#include <kvs/glut/Axis2DRenderer>
#include <kvs/glew/PointRenderer>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>

#include <stdlib.h>
#include <time.h>

#include "MDSTable.h"

int main( int argc, char** argv )
{
    // kvs::TableObject* input = new kvs::TableImporter( argv[1] );
    
    // size_t n = input->nrows();
    // size_t m = input->ncolumns();
    // double dsmatrix[n*m];

    std::string title = "MDS";

    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );

    /*
    for (size_t i=0; i<n; i++)
    {
        for (size_t j=0; j<m; j++)
        {
            dsmatrix[i*m+j] = input->column(j).to<double>(i);
        }
    }
    */
    /*
    for(size_t i=0; i<n; i++)
    {
        for(size_t j=0; j<m; j++)
        {
            std::cout<<dsmatrix[i*m+j]<<" ";
        }
        std::cout<<std::endl;
    }
    */
    
    size_t n = 1000;
    srand(time(NULL));
    double* dsmatrix = new double[n*n];
    memset(dsmatrix, 0, n*n*sizeof(double));
    for (size_t i=1; i<n; i++)
    {
        for (size_t j=0; j<i; j++)
        {
            dsmatrix[i*n+j] = rand() % 1000;
            dsmatrix[j*n+i] = dsmatrix[i*n+j];
        }
    }

    mds::MDSTable mds2d;
    kvs::TableObject table;
    kvs::Timer timer;
    timer.start();
    table = mds2d.exec(dsmatrix, n);
    timer.stop();
    std::cout << "Duration Time: " << timer.msec() << " [msec]" << std::endl;
    // delete input;
    delete []dsmatrix;
    kvs::ScatterPlotRenderer* renderer01 = new kvs::ScatterPlotRenderer();
    kvs::glut::Axis2DRenderer* renderer02 = new kvs::glut::Axis2DRenderer();
    renderer01 -> disableShading();
    renderer01 -> setPointSize(6);
    renderer02 -> disableShading();
    screen.registerObject( &table, renderer01 );
    screen.registerObject( &table, renderer02 );
    screen.setGeometry( 0, 0, 848, 480 );
    screen.setTitle( title );
	screen.show();
    return( app.run() );
}
