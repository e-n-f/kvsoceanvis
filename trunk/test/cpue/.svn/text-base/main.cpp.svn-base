#include <iostream>
#include <string>
#include <sstream>
#include <kvs/File>
#include <kvs/CSV>
#include <kvs/ValueArray>
#include <kvs/TableObject>
#include <util/TableObjectWriter.h>


size_t NumberOfColumns( const kvs::Csv& csv )
{
    size_t counter = 0;
    const size_t ncolumns = csv.row(0).size();
    for ( size_t i = 0; i < ncolumns; i++, counter++ )
    {
        if ( csv.row(0).at(i).size() == 0 ) break;
    }

    return counter;
}

size_t NumberOfRows( const kvs::Csv& csv )
{
    size_t counter = 0;
    const size_t nrows = csv.nrows();
    for ( size_t i = 0; i < nrows; i++, counter++ )
    {
        bool empty = true;
        for ( size_t j = 0; j < csv.row(i).size(); j++ )
        {
            if ( csv.row(i).at(j).size() != 0 ) { empty = false; break; }
        }

        if ( empty ) break;
    }

    return counter;
}

size_t IndexOf( const kvs::Csv& csv, const std::string& title )
{
    size_t counter = 0;
    const size_t ncolumns = csv.row(0).size();
    for ( size_t i = 0; i < ncolumns; i++, counter++ )
    {
        if ( csv.row(0).at(i) == title ) break;
    }

    return counter;
}

template <typename T>
kvs::ValueArray<T> ColumnArray( const kvs::Csv& csv, const std::string& title )
{
    const size_t index = IndexOf( csv, title );
    if ( index == NumberOfColumns( csv ) )
    {
        std::cerr << "Cannot find " << title << "." << std::endl;
        return kvs::ValueArray<T>();
    }

    const size_t nrows = NumberOfRows( csv );
    kvs::ValueArray<T> array( nrows - 1 );
    for ( size_t i = 0; i < nrows - 1; i++ )
    {
        std::stringstream value;
        value << csv.row( i + 1 ).at( index );
        value >> array[i];
    }

    return array;
}

template <typename T>
kvs::ValueArray<T> ColumnArrayOfDate( const kvs::Csv& csv, const std::string& title )
{
    const size_t index = IndexOf( csv, title );
    if ( index == NumberOfColumns( csv ) )
    {
        std::cerr << "Cannot find " << title << "." << std::endl;
        return kvs::ValueArray<T>();
    }

    const size_t nrows = NumberOfRows( csv );
    kvs::ValueArray<T> array( nrows - 1 );
    for ( size_t i = 0; i < nrows - 1; i++ )
    {
        std::istringstream stream( csv.row( i + 1 ).at( index ) );
        std::string date; std::getline( stream, date, ' ' );

//        char sep = '.';
        char sep = '/';
        std::istringstream date_stream( date );
        std::string syear; std::getline( date_stream, syear, sep );
        std::string smonth; std::getline( date_stream, smonth, sep );
        std::string sday; std::getline( date_stream, sday, sep );

        int year = std::atoi( syear.c_str() );
        int month = std::atoi( smonth.c_str() );
        int day = std::atoi( sday.c_str() );

        char svalue[9];
        std::sprintf( svalue, "%04d%02d%02d", year, month, day );

        std::stringstream value;
        value << svalue;
        value >> array[i];
    }

    return array;
}

int main( int argc, char** argv )
{
    std::string filename = argv[1];

    kvs::Csv csv( filename );
    kvs::ValueArray<kvs::Real32> lat = ColumnArray<kvs::Real32>( csv, "Dec Lat" );
    kvs::ValueArray<kvs::Real32> lon = ColumnArray<kvs::Real32>( csv, "Dec Long Corerct" );
    kvs::ValueArray<kvs::Real32> sst = ColumnArray<kvs::Real32>( csv, "SST oC" );
    kvs::ValueArray<kvs::Real32> start = ColumnArrayOfDate<kvs::Real32>( csv, "Start" );
    kvs::ValueArray<kvs::Real32> end = ColumnArrayOfDate<kvs::Real32>( csv, "End" );
    kvs::ValueArray<kvs::Real32> cpue = ColumnArray<kvs::Real32>( csv, "CPUE(squidN/hr/JNo)" );

    kvs::TableObject* table = new kvs::TableObject();
    table->addColumn( lat, "Lat" );
    table->addColumn( lon, "Lon" );
    table->addColumn( sst, "SST" );
    table->addColumn( start, "Start" );
    table->addColumn( end, "End" );
    table->addColumn( cpue, "CPUE" );

    kvsoceanvis::util::TableObjectWriter* writer = new kvsoceanvis::util::TableObjectWriter( table );
    writer->write( kvs::File( filename ).baseName() + ".kvsml" );

    delete table;
    delete writer;

    return 0;
}
