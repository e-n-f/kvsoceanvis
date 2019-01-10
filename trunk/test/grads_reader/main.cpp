#include <string>
#include <iostream>
#include <kvs/GrADS>


int main( int argc, char** argv )
{
    kvs::GrADS grads( argv[1] );

    const size_t dimx = grads.dataDescriptor().xdef().num;
    const size_t dimy = grads.dataDescriptor().ydef().num;
    const size_t dimz = grads.dataDescriptor().zdef().num;
    const size_t dimt = grads.dataList().size();
    const size_t nvars = grads.dataDescriptor().vars().values.size();

    std::cout << "Title: " << grads.dataDescriptor().title().name << std::endl;
    std::cout << "Number of grid points in the x direction: " << dimx << std::endl;
    std::cout << "Number of grid points in the y direction: " << dimy << std::endl;
    std::cout << "Number of grid points in the z direction: " << dimz << std::endl;
    std::cout << "Number of time steps: " << dimt << std::endl;
    std::cout << "Number of variables: " << nvars << std::endl;

    for ( size_t tindex = 0; tindex < dimt; ++tindex )
    {
        std::cout << "Filename[" << tindex << "]: " << grads.dataList().at(tindex).filename() << std::endl;
    }

    return 0;
}
