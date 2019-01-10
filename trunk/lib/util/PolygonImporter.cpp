/*****************************************************************************/
/**
 *  @file   PolygonImporter.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: PolygonImporter.cpp 287 2012-06-15 14:20:06Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "PolygonImporter.h"
#include <kvs/Vector3>
#include <kvs/RGBColor>
#include <kvs/ColorMap>


namespace
{

size_t GetMaxLatitudeIndex( const kvs::gis::Mesh* mesh )
{
    return( mesh->latitudeDimension() - 1 );
}

size_t GetMaxLongitudeIndex( const kvs::gis::Mesh* mesh )
{
    return( mesh->longitudeDimension() - 1 );
}

size_t GetNearestLatitudeIndex(
    const kvs::Vector2f& latitude,
    const size_t dim,
    const float interval,
    const float lat )
{
    size_t ret = 0;
    if ( ( lat < latitude.x() ) || kvs::Math::Equal( lat, latitude.x() ) ) { ret = dim - 1; }
    else if ( ( lat > latitude.y() ) || kvs::Math::Equal( lat, latitude.y() ) ) { ret = 0; }
    else
    {
        ret = dim - static_cast<size_t>( ( lat - latitude.x() ) / interval );
    }

    return( ret );
}

size_t GetNearestLongitudeIndex(
    const kvs::Vector2f& longitude,
    const size_t dim,
    const float interval,
    const float lon )
{
    size_t ret = 0;
    if ( ( lon < longitude.x() ) || kvs::Math::Equal( lon, longitude.x() ) )
    {
        ret = 0;
    }
    else if ( ( lon > longitude.y() ) || kvs::Math::Equal( lon, longitude.y() )  )
    {
        ret = dim - 1;
    }
    else
    {
        ret = static_cast<size_t>( ( lon - longitude.x() ) / interval );
    }

    return( ret );
}

kvs::Vector4ui GetArea(
    const kvs::gis::Mesh* mesh,
    const kvs::Vector2ui& min_range,
    const kvs::Vector2ui& max_range )
{
    size_t min_lat = min_range.x();
    size_t min_lon = min_range.y();
    size_t max_lat = max_range.x();
    size_t max_lon = max_range.y();

    if ( min_lat < 0 ) { min_lat = 0; }
    if ( min_lon < 0 ) { min_lon = 0; }

    const size_t lat = GetMaxLatitudeIndex( mesh );
    const size_t lon = GetMaxLongitudeIndex( mesh );
    if ( max_lat > lat ) { max_lat = lat; }
    if ( max_lon > lon ) { max_lon = lon; }

    return( kvs::Vector4ui( min_lat, min_lon, max_lat, max_lon ) );
}

kvs::Vector4ui GetArea(
    const kvs::gis::Mesh* mesh,
    const kvs::Vector2f& min_range,
    const kvs::Vector2f& max_range,
    const kvs::Vector2f& latitude,
    const kvs::Vector2f& longitude,
    const size_t lat_dim,
    const size_t lon_dim,
    const size_t lat_interval,
    const size_t lon_interval )
{
    const kvs::Vector2ui min_id(
        GetNearestLatitudeIndex( latitude, lat_dim, lat_interval, max_range.x() ),
        GetNearestLongitudeIndex( longitude, lon_dim, lon_interval, min_range.y() ) );

    const kvs::Vector2ui max_id(
        GetNearestLatitudeIndex( latitude, lat_dim, lat_interval, min_range.x() ),
        GetNearestLongitudeIndex( longitude, lon_dim, lon_interval, max_range.y() ) );

    return( GetArea( mesh, min_id, max_id ) );
}

}

namespace kvsoceanvis
{

namespace util
{

PolygonImporter::PolygonImporter( const std::string& filename )
{
    kvs::Gis* file_format = new kvs::Gis( filename );
    this->import( file_format );
    delete file_format;
}

PolygonImporter::PolygonImporter( const kvs::FileFormatBase* file_format )
{
    this->exec( file_format );
}

kvs::PolygonObject* PolygonImporter::exec( const kvs::FileFormatBase* file_format )
{
    this->import( static_cast<const kvs::Gis*>( file_format ) );
    return( this );
}

void PolygonImporter::import( const kvs::Gis* gis )
{
    std::vector<kvs::Real32> coords;
    std::vector<kvs::UInt32> connections;
    std::vector<kvs::UInt8> colors;
    std::vector<kvs::Real32> normals;

    const size_t nmeshes = gis->meshes().size();
    for ( size_t m = 0; m < nmeshes; m++ )
    {
        const kvs::gis::Mesh* mesh = gis->mesh( m );

        const size_t latitude_dim  = mesh->latitudeDimension();
        const size_t longitude_dim = mesh->longitudeDimension();

        kvs::Vector3f** position = new kvs::Vector3f* [ latitude_dim ];
        kvs::RGBColor** color = new kvs::RGBColor* [ latitude_dim ];
        bool** isocean = new bool* [ latitude_dim ];
        size_t** index = new size_t* [ latitude_dim ];
        for ( size_t i = 0; i < latitude_dim; i++ )
        {
            position[i] = new kvs::Vector3f [ longitude_dim ];
            color[i] = new kvs::RGBColor [ longitude_dim ];
            isocean[i] = new bool [ longitude_dim ];
            index[i] = new size_t [ longitude_dim ];
        }

        const float min_latitude = mesh->area().minLatitude();
        const float max_latitude = mesh->area().maxLatitude();
        const float min_longitude = mesh->area().minLongitude();
        const float max_longitude = mesh->area().maxLongitude();

        const float latitude_interval = ( max_latitude - min_latitude ) / (float)( latitude_dim - 1 );
        const float longitude_interval = ( max_longitude - min_longitude ) / (float)( longitude_dim - 1 );

        const float ratio_lat = (float)( latitude_dim - 1 ) / (float)( ::GetMaxLatitudeIndex( mesh ) );
        const float ratio_lon = (float)( longitude_dim - 1 ) / (float)( ::GetMaxLongitudeIndex( mesh ) );

        float min_height =  9999.0f;
        float max_height = -9999.0f;
        for ( size_t j = 0; j < latitude_dim; j++ )
        {
            const float v = static_cast<float>(j) / ratio_lat;
            const size_t pv1 = kvs::Math::Floor(v);
            const size_t pv2 = pv1 == ( mesh->latitudeDimension() - 1 ) ? pv1 : pv1 + 1;
            const float yrate = v - static_cast<float>( pv1 );

            const float latitude = max_latitude - latitude_interval * j;
            for ( size_t i = 0; i < longitude_dim; i++ )
            {
                /* On 'data'
                 *
                 *      p1----------p3   p': (u,v)
                 *      |           |    p1: (pu1,pv1), color1
                 *      |           |    p2: (pu1,pv2), color2
                 *      |    p'     |    p3: (pu2,pv1), color3
                 *      |           |    p4: (pu2,pv2), color4
                 *      p2----------p4
                 */
                const float u = static_cast<float>(i) / ratio_lon;
                const size_t pu1 = kvs::Math::Floor(u);
                const size_t pu2 = pu1 == ( mesh->longitudeDimension() - 1 ) ? pu1 : pu1 + 1;
                const float xrate = u - static_cast<float>( pu1 );

                const float height1 = mesh->data( pv1, pu1 ).height();
                const float height2 = mesh->data( pv1, pu2 ).height();
                const float height3 = mesh->data( pv2, pu1 ).height();
                const float height4 = mesh->data( pv2, pu2 ).height();
                const float longitude = min_longitude + longitude_interval * i;

                float height = 0.0f;
                if ( kvs::Math::Equal( height1, kvs::gis::OceanValue ) ||
                     kvs::Math::Equal( height2, kvs::gis::OceanValue ) ||
                     kvs::Math::Equal( height3, kvs::gis::OceanValue ) ||
                     kvs::Math::Equal( height4, kvs::gis::OceanValue ) )
                {
                    isocean[j][i] = true;
                }
                else
                {
                    isocean[j][i] = false;

                    // Bilinear interpolation.
                    const float d = height1 * ( 1.0f - xrate ) + height2 * xrate;
                    const float e = height3 * ( 1.0f - xrate ) + height4 * xrate;
                    height = d * ( 1.0f - yrate ) + e * yrate;

                    min_height = kvs::Math::Min( min_height, height );
                    max_height = kvs::Math::Max( max_height, height );
                }

                const float x = longitude * 91.0f;
                const float y = latitude * 111.0f;
                const float z = height;
                position[j][i] = kvs::Vector3f( x, y, z ); // (km, km, km)
                //position[j][i] = kvs::Vector3f( longitude, latitude, height ); // (deg, deg, km)
            }
        }

        min_height = kvs::Math::Equal( min_height,  9999.0f ) ? kvs::gis::OceanValue / 10000.0f : min_height;
        max_height = kvs::Math::Equal( max_height, -9999.0f ) ? kvs::gis::OceanValue / 10000.0f : max_height;

        // Set color map.
        kvs::ColorMap color_map( 256 );
        color_map.setRange( min_height, max_height );
        color_map.create();

        for ( size_t row = 0; row < latitude_dim; row++ )
        {
            for ( size_t col = 0; col < longitude_dim; col++ )
            {
                // Set data.
                if( !isocean[row][col] )
                {
                    color[row][col] = color_map.at( position[row][col].z() );
                }
                else
                {
//                        color[row][col] = kvs::gis::OceanColor;
                    color[row][col] = gis->oceanColor();
                    position[row][col][2] = gis->area().minHeight();
                }
            }
        }

        const kvs::Vector4ui range_index =
            ::GetArea( mesh,
                       kvs::Vector2f( gis->renderArea().minLatitude(), gis->renderArea().minLongitude() ),
                       kvs::Vector2f( gis->renderArea().maxLatitude(), gis->renderArea().maxLongitude() ),
                       kvs::Vector2f( min_latitude, max_latitude ),
                       kvs::Vector2f( min_longitude, max_longitude ),
                       latitude_dim, longitude_dim,
                       latitude_interval, longitude_interval );

        const size_t min_lat = range_index.x();
        const size_t min_lon = range_index.y();
        const size_t max_lat = range_index.z();
        const size_t max_lon = range_index.w();
        if ( ( min_lat == max_lat ) || ( min_lon == max_lon ) ) continue;

        for ( size_t lat = min_lat; lat <= max_lat; lat++ )
        {
            for ( size_t lon = min_lon; lon <= max_lon; lon++ )
            {
                coords.push_back( position[lat][lon].x() );
                coords.push_back( position[lat][lon].y() );
                coords.push_back( position[lat][lon].z() );
                colors.push_back( color[lat][lon].r() );
                colors.push_back( color[lat][lon].g() );
                colors.push_back( color[lat][lon].b() );

                index[lat][lon] = coords.size() / 3 - 1;
            }
        }

        for ( size_t lat = min_lat; lat < max_lat; lat++ )
        {
            for ( size_t lon = min_lon; lon < max_lon; lon++ )
            {
                if ( isocean[lat][lon] ) continue;

                connections.push_back( index[lat][lon]     );
                connections.push_back( index[lat][lon+1]   );
                connections.push_back( index[lat+1][lon+1] );
                connections.push_back( index[lat+1][lon]   );

                const kvs::Vector3f v1 = position[lat+1][lon] - position[lat][lon];
                const kvs::Vector3f v2 = position[lat][lon+1] - position[lat][lon];
                const kvs::Vector3f norm = v2.cross( v1 );
                normals.push_back( norm.x() );
                normals.push_back( norm.y() );
                normals.push_back( norm.z() );
            }
        }

        for ( size_t i = 0; i < latitude_dim; i++ )
        {
            delete [] position[i];
            delete [] color[i];
            delete [] isocean[i];
            delete [] index[i];
        }
        delete [] position;
        delete [] color;
        delete [] isocean;
        delete [] index;
    }

    setPolygonType( kvs::PolygonObject::Quadrangle );
    setColorType( kvs::PolygonObject::VertexColor );
    setNormalType( kvs::PolygonObject::PolygonNormal );
    setCoords( kvs::ValueArray<kvs::Real32>(coords) );
    setColors( kvs::ValueArray<kvs::UInt8>(colors) );
    setNormals( kvs::ValueArray<kvs::Real32>(normals) );
    setConnections( kvs::ValueArray<kvs::UInt32>(connections) );
    setOpacity( 255 );
    updateMinMaxCoords();
}

} // end of namespace util

} // end of namespace kvsoceanvis
