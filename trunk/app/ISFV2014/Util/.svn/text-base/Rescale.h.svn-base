#pragma once

#include <kvs/Assert>
#include <kvs/Vector3>
#include <kvs/StructuredVolumeObject>


namespace ISFV2014
{

namespace Rescale
{

void Coords( kvs::StructuredVolumeObject* volume, kvs::Vec3 scale )
{
    KVS_ASSERT( volume->gridType() == kvs::StructuredVolumeObject::Rectilinear );

    const size_t dimx = volume->resolution().x();
    const size_t dimy = volume->resolution().y();
    const size_t dimz = volume->resolution().z();
    kvs::ValueArray<kvs::Real32> coords = volume->coords();
    kvs::Real32* xcoord = coords.data();
    kvs::Real32* ycoord = coords.data() + dimx;
    kvs::Real32* zcoord = coords.data() + dimx + dimy;

    const kvs::Real32 x0 = xcoord[0];
    for ( size_t i = 0; i < dimx; i++ )
    {
        const kvs::Real32 x = xcoord[i];
        xcoord[i] = ( x - x0 ) * scale.x() + x0;
    }

    const kvs::Real32 y0 = ycoord[0];
    for ( size_t j = 0; j < dimy; j++ )
    {
        const kvs::Real32 y = ycoord[j];
        ycoord[j] = ( y - y0 ) * scale.y() + y0;
    }

    const kvs::Real32 z0 = zcoord[0];
    for ( size_t k = 0; k < dimz; k++ )
    {
        const kvs::Real32 z = zcoord[k];
        zcoord[k] = ( z - z0 ) * scale.z() + z0;
    }

    kvs::Vec3 min_coord( xcoord[0], ycoord[0], zcoord[0] );
    kvs::Vec3 max_coord( xcoord[dimx-1], ycoord[dimy-1], zcoord[dimz-1] );
    volume->setMinMaxObjectCoords( min_coord, max_coord );
    volume->setMinMaxExternalCoords( min_coord, max_coord );
};

void Values( kvs::StructuredVolumeObject* volume, kvs::Vec3 scale, bool update_by_horizontal = false )
{
    KVS_ASSERT( volume->veclen() == 3 );

    const size_t nnodes = volume->numberOfNodes();
    kvs::ValueArray<kvs::Real32> values = volume->values().asValueArray<kvs::Real32>();
    for ( size_t i = 0; i < nnodes; i++ )
    {
        values[ 3 * i + 0 ] *= scale.x();
        values[ 3 * i + 1 ] *= scale.y();
        values[ 3 * i + 2 ] *= scale.z();
    }

    if ( update_by_horizontal )
    {
        const float* value = reinterpret_cast<const float*>( volume->values().data() );
        const float* const end = value + volume->numberOfNodes() * volume->veclen();

        kvs::Real64 min_value = kvs::Value<kvs::Real64>::Max();
        kvs::Real64 max_value = kvs::Value<kvs::Real64>::Min();
        while ( value < end )
        {
            kvs::Real64 magnitude = 0.0;
            for ( size_t i = 0; i < 2; ++i )
            {
                magnitude += static_cast<kvs::Real64>( ( *value ) * ( *value ) );
                ++value;
            }

            min_value = kvs::Math::Min( magnitude, min_value );
            max_value = kvs::Math::Max( magnitude, max_value );

            ++value;
        }

        volume->setMinMaxValues( std::sqrt( min_value ), std::sqrt( max_value ) );
    }
    else
    {
        volume->updateMinMaxValues();
    }
};

} // end of namespace Rescale

} // end of namespace ISFV2014
