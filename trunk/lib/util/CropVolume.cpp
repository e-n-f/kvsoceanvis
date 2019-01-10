/*****************************************************************************/
/**
 *  @file   CropVolume.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: CropVolume.cpp 360 2014-05-15 08:57:12Z naohisa.sakamoto@gmail.com $
 */
/*****************************************************************************/
#include "CropVolume.h"


namespace
{

size_t MinIndexOf( const float coord, const float* coords, const size_t coords_size )
{
    if ( coord < coords[0] ) { return 0; }
    if ( coords[ coords_size - 1 ] <= coord ) { return coords_size - 1; }

    for ( size_t i = 0; i < coords_size - 1; i++ )
    {
        if ( coords[i] <= coord && coord < coords[i+1] ) { return i; }
    }

    return coords_size - 1;
}

size_t MaxIndexOf( const float coord, const float* coords, const size_t coords_size )
{
    size_t index = MinIndexOf( coord, coords, coords_size ) + 1;
    return kvs::Math::Clamp( index, size_t(0), coords_size - 1 );
}

}


namespace kvsoceanvis
{

namespace util
{

CropVolume::CropVolume(
    const kvs::StructuredVolumeObject* volume,
    const kvs::Vector3f min_range,
    const kvs::Vector3f max_range )
{
    m_min_range = min_range;
    m_max_range = max_range;
    this->exec( volume );
}

CropVolume::SuperClass* CropVolume::exec( const kvs::ObjectBase* object )
{
    const kvs::StructuredVolumeObject* volume = kvs::StructuredVolumeObject::DownCast( object );

    if ( volume->gridType() == kvs::StructuredVolumeObject::Uniform )
    {
        const size_t imin = static_cast<size_t>( kvs::Math::Max( m_min_range.x(), 0.0f ) );
        const size_t jmin = static_cast<size_t>( kvs::Math::Max( m_min_range.y(), 0.0f ) );
        const size_t kmin = static_cast<size_t>( kvs::Math::Max( m_min_range.z(), 0.0f ) );

        const size_t imax = static_cast<size_t>( kvs::Math::Min( m_max_range.x(), volume->resolution().x() - 1.0f ) );
        const size_t jmax = static_cast<size_t>( kvs::Math::Min( m_max_range.y(), volume->resolution().y() - 1.0f ) );
        const size_t kmax = static_cast<size_t>( kvs::Math::Min( m_max_range.z(), volume->resolution().z() - 1.0f ) );

        const size_t dimx = imax - imin + 1;
        const size_t dimy = jmax - jmin + 1;
        const size_t dimz = kmax - kmin + 1;

        const size_t veclen = volume->veclen();
        const size_t line_size = volume->numberOfNodesPerLine();
        const size_t slice_size = volume->numberOfNodesPerSlice();
        const kvs::Real32* src = static_cast<const kvs::Real32*>( volume->values().data() );

        kvs::ValueArray<kvs::Real32> values( dimx * dimy * dimz * veclen );
        kvs::Real32* dst = values.data();
        for ( size_t k = kmin; k <= kmax; k++ )
        {
            for ( size_t j = jmin; j <= jmax; j++ )
            {
                for ( size_t i = imin; i <= imax; i++ )
                {
                    const size_t index = ( i + j * line_size + k * slice_size ) * veclen;
                    for ( size_t v = 0; v < veclen; v++ )
                    {
                        *(dst++) = src[ index + v ];
                    }
                }
            }
        }

        SuperClass::setGridType( volume->gridType() );
        SuperClass::setResolution( kvs::Vector3ui( dimx, dimy, dimz ) );
        SuperClass::setVeclen( volume->veclen() );
        SuperClass::setValues( kvs::AnyValueArray( values ) );
        SuperClass::updateMinMaxValues();
        SuperClass::updateMinMaxCoords();
    }
    else if ( volume->gridType() == kvs::StructuredVolumeObject::Rectilinear )
    {
        const kvs::Vec3 min_range(
            kvs::Math::Max( m_min_range.x(), volume->minObjectCoord().x() ),
            kvs::Math::Max( m_min_range.y(), volume->minObjectCoord().y() ),
            kvs::Math::Max( m_min_range.z(), volume->minObjectCoord().z() ) );

        const kvs::Vec3 max_range(
            kvs::Math::Min( m_max_range.x(), volume->maxObjectCoord().x() ),
            kvs::Math::Min( m_max_range.y(), volume->maxObjectCoord().y() ),
            kvs::Math::Min( m_max_range.z(), volume->maxObjectCoord().z() ) );

        const kvs::Real32* src_values = static_cast<const kvs::Real32*>( volume->values().data() );
        const kvs::Real32* src_xcoords = volume->coords().data();
        const kvs::Real32* src_ycoords = src_xcoords + volume->resolution().x();
        const kvs::Real32* src_zcoords = src_ycoords + volume->resolution().y();

        const size_t imin = ::MinIndexOf( min_range.x(), src_xcoords, volume->resolution().x() );
        const size_t jmin = ::MinIndexOf( min_range.y(), src_ycoords, volume->resolution().y() );
        const size_t kmin = ::MinIndexOf( min_range.z(), src_zcoords, volume->resolution().z() );

        const size_t imax = ::MaxIndexOf( max_range.x(), src_xcoords, volume->resolution().x() );
        const size_t jmax = ::MaxIndexOf( max_range.y(), src_ycoords, volume->resolution().y() );
        const size_t kmax = ::MaxIndexOf( max_range.z(), src_zcoords, volume->resolution().z() );

        const size_t dimx = imax - imin + 1;
        const size_t dimy = jmax - jmin + 1;
        const size_t dimz = kmax - kmin + 1;

        const size_t veclen = volume->veclen();
        const size_t line_size = volume->numberOfNodesPerLine();
        const size_t slice_size = volume->numberOfNodesPerSlice();

        kvs::ValueArray<kvs::Real32> values( dimx * dimy * dimz * veclen );
        kvs::Real32* dst_values = values.data();
        for ( size_t k = kmin; k <= kmax; k++ )
        {
            for ( size_t j = jmin; j <= jmax; j++ )
            {
                for ( size_t i = imin; i <= imax; i++ )
                {
                    const size_t index = ( i + j * line_size + k * slice_size ) * veclen;
                    for ( size_t v = 0; v < veclen; v++ )
                    {
                        *(dst_values++) = src_values[ index + v ];
                    }
                }
            }
        }

        kvs::ValueArray<kvs::Real32> coords( dimx + dimy + dimz );
        kvs::Real32* dst_coords = coords.data();
        for ( size_t i = imin; i <= imax; i++ ) { *(dst_coords++) = src_xcoords[i]; }
        for ( size_t j = jmin; j <= jmax; j++ ) { *(dst_coords++) = src_ycoords[j]; }
        for ( size_t k = kmin; k <= kmax; k++ ) { *(dst_coords++) = src_zcoords[k]; }

        SuperClass::setGridType( kvs::StructuredVolumeObject::Rectilinear );
        SuperClass::setResolution( kvs::Vector3ui( dimx, dimy, dimz ) );
        SuperClass::setVeclen( volume->veclen() );
        SuperClass::setCoords( coords );
        SuperClass::setValues( kvs::AnyValueArray( values ) );
        SuperClass::updateMinMaxValues();
        SuperClass::updateMinMaxCoords();
    }

    return this;
}

} // end of namespace util

} // end of namespace kvsoceanvis
