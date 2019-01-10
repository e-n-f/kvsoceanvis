#include <iostream>
#include <string>
#include <kvs/File>
#include <QApplication>
#include "CPUEData.h"


int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    std::string filename( argv[1] );

    std::cout << filename << std::endl;
    std::cout << kvs::File( filename ).baseName() + ".sqlite3" << std::endl;

    CPUEData cpue_data( filename );
    if ( cpue_data.open( kvs::File( filename ).baseName() + ".sqlite3" ) )
    {
        cpue_data.create();
    }
    else std::cout << "Cannot open DB file." << std::endl;

    return app.exec();
}
