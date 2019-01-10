#pragma once


#include <iostream>
#include <fstream>
#include <vector>
#include <kvs/Csv>
#include <kvs/String>


namespace ISFV2014
{

class CoastlJpn
{
    typedef kvs::ValueArray<float> Coords;
    std::vector<Coords> m_coords_array;

public:

    CoastlJpn( const std::string filename )
    {
        kvs::Csv csv( filename );

        const size_t nrows = csv.nrows();
        for ( size_t i = 0; i < nrows; i++ )
        {
            const size_t ncolumns = csv.row(i).size();
            Coords coords( ncolumns );
            for ( size_t j = 0; j < ncolumns; j++ )
            {
                coords[j] = kvs::String::To<float>( csv.row(i).at(j) );
            }
            m_coords_array.push_back( coords );
        }
    }

    kvs::LineObject* toLineObject(
        const float depth,
        const float longitude_scale = 91.0f,
        const float latitude_scale = 111.0f )
    {
        const size_t nlines = m_coords_array.size();
        std::vector<kvs::Real32> coords;
        std::vector<kvs::UInt32> connections;
        size_t start = 0;
        size_t end = 0;
        for ( size_t i = 0; i < nlines; i++ )
        {
            const size_t nvertices = m_coords_array[i].size() / 2;
            end = start + nvertices - 1;
            connections.push_back( start );
            connections.push_back( end );
            for ( size_t j = 0; j < nvertices; j++ )
            {
                coords.push_back( m_coords_array[i][ 2 * j + 1 ] * longitude_scale );
                coords.push_back( m_coords_array[i][ 2 * j + 0 ] * latitude_scale );
                coords.push_back( depth );
            }
            start = end + 1;
        }

        kvs::LineObject* line = new kvs::LineObject();
        line->setLineType( kvs::LineObject::Polyline );
        line->setColorType( kvs::LineObject::LineColor );
        line->setCoords( kvs::ValueArray<kvs::Real32>( coords ) );
        line->setConnections( kvs::ValueArray<kvs::UInt32>( connections ) );
        line->setColor( kvs::RGBColor::Black() );
        line->setSize( 1.0 );
        line->updateMinMaxCoords();
        return line;
    }
};

} // end of namespace ISFV2014
