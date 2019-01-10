#include "CPUEData.h"
#include <sstream>
#include <string>
#include <kvs/CSV>
#include <kvs/ValueArray>
#include <kvs/TableObject>
#include <kvs/String>
#include <QtSql>
#include <QDebug>

namespace
{

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

}

CPUEData::CPUEData( const std::string& filename )
{
    kvs::Csv input( filename );
    kvs::ValueArray<kvs::Real32> lat = ::ColumnArray<kvs::Real32>( input, "Dec Lat" );
    kvs::ValueArray<kvs::Real32> lon = ::ColumnArray<kvs::Real32>( input, "Dec Long Corerct" );
    kvs::ValueArray<kvs::Real32> sst = ::ColumnArray<kvs::Real32>( input, "SST oC" );
    kvs::ValueArray<kvs::Real32> start = ::ColumnArrayOfDate<kvs::Real32>( input, "Start" );
    kvs::ValueArray<kvs::Real32> end = ::ColumnArrayOfDate<kvs::Real32>( input, "End" );
    kvs::ValueArray<kvs::Real32> cpue = ::ColumnArray<kvs::Real32>( input, "CPUE(squidN/hr/JNo)" );

    m_table.addColumn( start, "start" );
    m_table.addColumn( end, "end" );
    m_table.addColumn( lon, "longitude" );
    m_table.addColumn( lat, "latitude" );
    m_table.addColumn( sst, "sst" );
    m_table.addColumn( cpue, "cpue" );

    m_database = QSqlDatabase::addDatabase("QSQLITE");
}

bool CPUEData::open( const std::string& dbname )
{
    m_database.setDatabaseName( dbname.c_str() );
    return m_database.open();
}

void CPUEData::close()
{
    m_database.close();
}

bool CPUEData::create()
{
    if ( !m_database.isOpen() )
    {
        qDebug() << "The database connection isn't open.";
        return false;
    }

    QSqlQuery query;

    query.prepare( "CREATE TABLE IF NOT EXISTS cpue ("
                   "start INTEGER,"
                   "end INTEGER,"
                   "longitude REAL,"
                   "latitude REAL,"
                   "sst REAL,"
                   "cpue REAL)");
    if ( !query.exec() ) qDebug() << query.lastError(); else qDebug() << "Table created";

    for ( size_t i = 0; i < m_table.numberOfRows(); i++ )
    {
        const int start = m_table.column(0).at<int>(i);
        const int end = m_table.column(1).at<int>(i);
        const float longitude = m_table.column(2).at<float>(i);
        const float latitude = m_table.column(3).at<float>(i);
        const float sst = m_table.column(4).at<float>(i);
        const float cpue = m_table.column(5).at<float>(i);

        const std::string q =
            std::string("INSERT INTO cpue ("
                        "start,"
                        "end,"
                        "longitude,"
                        "latitude,"
                        "sst,"
                        "cpue)") +
            std::string("VALUES (") +
            kvs::String::ToString(start) + std::string(",") +
            kvs::String::ToString(end) + std::string(",") +
            kvs::String::ToString(longitude) + std::string(",") +
            kvs::String::ToString(latitude) + std::string(",") +
            kvs::String::ToString(sst) + std::string(",") +
            kvs::String::ToString(cpue) + std::string(")");
        query.prepare( QString( q.c_str() ) );
        if ( !query.exec() ) qDebug() << query.lastError();
        else qDebug() << "Inserted ("
                      << start << ","
                      << end << ","
                      << longitude << ","
                      << latitude << ","
                      << sst << ","
                      << cpue << ")";
    }

    m_database.close();

    return true;
}
