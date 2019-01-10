#include <iostream>
#include <fstream>
#include <kvs/Timer>
#include <kvs/TableImporter>
#include "KCluster.h"

int main( int argc, char** argv )
{
    //kvs::TableObject* input = new kvs::TableImporter( "data/test.kvsml" );
    kvs::TableObject* input = new kvs::TableImporter( argv[1] );
    int k = 10;
    kvs::KCluster hspkmeans(k);
    std::ofstream outfile("output.txt");
    srand(time(NULL));
    kvs::Timer timer;
    timer.start();
    kvs::TableObject output = hspkmeans.exec(*input);
    kvs::ValueArray<int> ClusterIDs = hspkmeans.getClusterIDs();
    timer.stop();
    std::cout << "Duration Time: " << timer.msec() << " [msec]" << std::endl;
    for (size_t i=0; i<ClusterIDs.size(); i++)
    {
        outfile << ClusterIDs[i] << std::endl;
    }
    outfile.close();
    return 0;
}
