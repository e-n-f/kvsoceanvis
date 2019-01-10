/*****************************************************************************/
/**
 *  @file   AdjustVectorScale.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
#include "AdjustVectorScale.h"


namespace kvsoceanvis
{

namespace util
{

const kvs::Real32 AdjustVectorScale::EarthRadius = 6378150.0f;

AdjustVectorScale::AdjustVectorScale(
    const kvs::StructuredVolumeObject* volume,
    const kvs::Real32 earth_radius,
    const kvs::Real32 depth_scale ):
    m_earth_radius( earth_radius ),
    m_depth_scale( depth_scale )
{
    this->exec( volume );
}

AdjustVectorScale::SuperClass* AdjustVectorScale::exec( const kvs::ObjectBase* object )
{
    const kvs::StructuredVolumeObject* volume = kvs::StructuredVolumeObject::DownCast( object );

    if ( volume->veclen() != 3 )
    {
        kvsMessageError( "Input volume is not vector volume." );
        return NULL;
    }

    if ( volume->gridType() != kvs::StructuredVolumeObject::Rectilinear )
    {
        kvsMessageError( "Input volume is not rectilinear grid volume." );
        return NULL;
    }

    kvs::AnyValueArray values = volume->values();
    kvs::Real32* pvalues = static_cast<kvs::Real32*>( values.data() );

    const size_t xdim = volume->resolution().x();
    const size_t ydim = volume->resolution().y();
    const size_t zdim = volume->resolution().z();
    const kvs::Real32* xcoords = volume->coords().data();
    const kvs::Real32* ycoords = xcoords + volume->resolution().x();
    const kvs::Real32* zcoords = ycoords + volume->resolution().y();

    const kvs::Real32 P = 3.14159265f;
    const kvs::Real32 R1 = m_earth_radius;
    const kvs::Real32 L1 = 2.0f * P * R1 / 360.0f;
    for ( size_t z = 0, index = 0; z < zdim; z++ )
    {
        kvs::Real32 zdiff = 0.0f;
        if ( z == 0 )
        {
            const kvs::Real32 z0 = zcoords[0] * m_depth_scale;
            const kvs::Real32 z1 = zcoords[1] * m_depth_scale;
            zdiff = kvs::Math::Abs( z1 - z0 );
        }
        else if ( z == zdim - 2 )
        {
            const kvs::Real32 z0 = zcoords[ zdim - 2 ] * m_depth_scale;
            const kvs::Real32 z1 = zcoords[ zdim - 1 ] * m_depth_scale;
            zdiff = kvs::Math::Abs( z1 - z0 );
        }
        else
        {
            const kvs::Real32 z0 = zcoords[ z - 1 ] * m_depth_scale;
            const kvs::Real32 z1 = zcoords[ z + 1 ] * m_depth_scale;
            zdiff = 0.5f * kvs::Math::Abs( z1 - z0 );
        }
        for ( size_t y = 0; y < ydim; y++ )
        {
            kvs::Real32 ydiff = 0.0f;
            if ( y == 0 )
            {
                const kvs::Real32 y0 = ycoords[0];
                const kvs::Real32 y1 = ycoords[1];
                ydiff = kvs::Math::Abs( y1 - y0 ) * L1;
            }
            else if ( y == ydim - 2 )
            {
                const kvs::Real32 y0 = ycoords[ ydim - 2 ];
                const kvs::Real32 y1 = ycoords[ ydim - 1 ];
                ydiff = kvs::Math::Abs( y1 - y0 ) * L1;
            }
            else
            {
                const kvs::Real32 y0 = ycoords[ y - 1 ];
                const kvs::Real32 y1 = ycoords[ y ];
                const kvs::Real32 y2 = ycoords[ y + 1 ];
                ydiff = 0.5f * ( kvs::Math::Abs( y1 - y0 ) + kvs::Math::Abs( y2 - y1 ) ) * L1;
            }

            const kvs::Real32 R2 = R1 * std::cos( ycoords[y] / 180.0f * P );
            const kvs::Real32 L2 = 2.0f * P * R2 / 360.0f;
            for ( size_t x = 0; x < xdim; x++, index++ )
            {
                kvs::Real32 xdiff = 0.0f;
                if ( x == 0 )
                {
                    const kvs::Real32 x0 = xcoords[0];
                    const kvs::Real32 x1 = xcoords[1];
                    xdiff = kvs::Math::Abs( x1 - x0 ) * L2;
                }
                else if ( x == xdim - 2 )
                {
                    const kvs::Real32 x0 = xcoords[ xdim - 2 ];
                    const kvs::Real32 x1 = xcoords[ xdim - 1 ];
                    xdiff = kvs::Math::Abs( x1 - x0 ) * L2;
                }
                else
                {
                    const kvs::Real32 x0 = xcoords[ x - 1 ];
                    const kvs::Real32 x1 = xcoords[ x ];
                    const kvs::Real32 x2 = xcoords[ x + 1 ];
                    xdiff = 0.5f * ( kvs::Math::Abs( x1 - x0 ) + kvs::Math::Abs( x2 - x1 ) ) * L2;
                }

                pvalues[ 3 * index + 0 ] /= xdiff;
                pvalues[ 3 * index + 1 ] /= ydiff;
                pvalues[ 3 * index + 2 ] /= zdiff;
            }
        }
    }

    SuperClass::setGridType( kvs::StructuredVolumeObject::Uniform );
    SuperClass::setResolution( volume->resolution() );
    SuperClass::setVeclen( 3 );
    SuperClass::setValues( values );
    SuperClass::updateMinMaxValues();
    SuperClass::updateMinMaxCoords();

    return this;
}

} // end of namespace util

} // end of namespace kvsoceanvis
