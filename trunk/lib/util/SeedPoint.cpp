/*****************************************************************************/
/**
 *  @file   SeedPoint.cpp
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
#include "SeedPoint.h"


namespace kvsoceanvis
{

namespace util
{

SeedPoint::SeedPoint( const kvs::PointObject* point, const float delta )
{
    m_delta = delta;
    this->exec( point );
}

SeedPoint::SuperClass* SeedPoint::exec( const kvs::ObjectBase* object )
{
    const kvs::PointObject* point = kvs::PointObject::DownCast( object );

    const size_t npoints = point->numberOfVertices();
    const size_t nseeds = npoints * 4;
    kvs::ValueArray<kvs::Real32> coords( nseeds * 3 );
    kvs::ValueArray<kvs::UInt8> colors( nseeds * 3 );
    kvs::Real32* pcoords = coords.pointer();
    kvs::UInt8* pcolors = colors.pointer();
    for ( size_t i = 0; i < npoints; i++ )
    {
        const kvs::Vector3f coord = point->coord( i );
        const kvs::Vector3f normal = point->normal( i );
        const kvs::RGBColor color = point->color( i );

        kvs::Vector3f vec1;
        kvs::Vector3f vec2;
        if ( normal.x() == 0 && normal.y() == 0 )
        {
            vec1 = kvs::Vector3f( 1, 0, 0 );
            vec2 = kvs::Vector3f( 0, 1, 0 );
        }
        else
        {
            vec1 = normal.cross(kvs::Vector3f(0,0,1));
            vec2 = vec1.cross(normal);
        }

        vec1.normalize();
        vec2.normalize();

        vec1 *= m_delta;
        vec2 *= m_delta;

        *(pcoords++) = coord.x() + vec1.x();
        *(pcoords++) = coord.y() + vec1.y();
        *(pcoords++) = coord.z() + vec1.z();
        *(pcolors++) = color.r();
        *(pcolors++) = color.g();
        *(pcolors++) = color.b();

        *(pcoords++) = coord.x() - vec1.x();
        *(pcoords++) = coord.y() - vec1.y();
        *(pcoords++) = coord.z() - vec1.z();
        *(pcolors++) = color.r();
        *(pcolors++) = color.g();
        *(pcolors++) = color.b();

        *(pcoords++) = coord.x() + vec2.x();
        *(pcoords++) = coord.y() + vec2.y();
        *(pcoords++) = coord.z() + vec2.z();
        *(pcolors++) = color.r();
        *(pcolors++) = color.g();
        *(pcolors++) = color.b();

        *(pcoords++) = coord.x() - vec2.x();
        *(pcoords++) = coord.y() - vec2.y();
        *(pcoords++) = coord.z() - vec2.z();
        *(pcolors++) = color.r();
        *(pcolors++) = color.g();
        *(pcolors++) = color.b();
    }

    SuperClass::setCoords( coords );
    SuperClass::setColors( colors );
    SuperClass::setSize( 1 );
    SuperClass::setMinMaxObjectCoords( point->minObjectCoord(), point->maxObjectCoord() );
    SuperClass::setMinMaxExternalCoords( point->minExternalCoord(), point->maxExternalCoord() );

    return this;
}

} // end of namespace util

} // end of namespace kvsoceanvis
