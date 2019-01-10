/*****************************************************************************/
/**
 *  @file   Helicity.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: Helicity.cpp 326 2012-12-22 06:19:41Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "Helicity.h"


namespace kvsoceanvis
{

namespace util
{

Helicity::Helicity( const kvs::StructuredVolumeObject* volume )
{
    this->exec( volume );
}

Helicity::SuperClass* Helicity::exec( const kvs::ObjectBase* object )
{
    const kvs::StructuredVolumeObject* volume = kvs::StructuredVolumeObject::DownCast( object );

    if ( volume->veclen() != 3 )
    {
        kvsMessageError( "Input volume is not vector volume." );
        return NULL;
    }

    if ( volume->gridType() != kvs::StructuredVolumeObject::Uniform )
    {
        kvsMessageError( "Input volume is not uniform grid." );
        return NULL;
    }

    const kvs::Real32* src_values = static_cast<const kvs::Real32*>( volume->values().data() );
    const kvs::Vector3ui resolution = volume->resolution();
    const kvs::UInt32 line_size = volume->numberOfNodesPerLine();
    const kvs::UInt32 slice_size = volume->numberOfNodesPerSlice();
    const size_t veclen = volume->veclen();

//    kvs::AnyValueArray values;
//    kvs::Real32* pvalues = static_cast<kvs::Real32*>( values.allocate<kvs::Real32>( volume->nnodes() ) );
    kvs::ValueArray<kvs::Real32> values( volume->numberOfNodes() );
    kvs::Real32* pvalues = values.data();
    size_t index = 0;
    for ( size_t k = 0; k < resolution.z(); k++ )
    {
        const int k0 = ( k == 0 ) ? 0 : -slice_size;
        const int k1 = ( k == resolution.z() - 1 ) ? 0 : +slice_size;

        const float u0z = src_values[ ( index + k0 ) * veclen + 0 ];
        const float u1z = src_values[ ( index + k1 ) * veclen + 0 ];
        const float dudz = ( u1z - u0z ) / 2.0f;

        const float v0z = src_values[ ( index + k0 ) * veclen + 1 ];
        const float v1z = src_values[ ( index + k1 ) * veclen + 1 ];
        const float dvdz = ( v1z - v0z ) / 2.0f;

        for ( size_t j = 0; j < resolution.y(); j++ )
        {
            const int j0 = ( j == 0 ) ? 0 : -line_size;
            const int j1 = ( j == resolution.y() - 1 ) ? 0 : +line_size;

            const float u0y = src_values[ ( index + j0 ) * veclen + 0 ];
            const float u1y = src_values[ ( index + j1 ) * veclen + 0 ];
            const float dudy = ( u1y - u0y ) / 2.0f;

            const float w0y = src_values[ ( index + j0 ) * veclen + 2 ];
            const float w1y = src_values[ ( index + j1 ) * veclen + 2 ];
            const float dwdy = ( w1y - w0y ) / 2.0f;

            for ( size_t i = 0; i < resolution.x(); i++, index++ )
            {
                const int i0 = ( i == 0 ) ? 0 : -1;
                const int i1 = ( i == resolution.x() - 1 ) ? 0 : +1;

                const float v0x = src_values[ ( index + i0 ) * veclen + 1 ];
                const float v1x = src_values[ ( index + i1 ) * veclen + 1 ];
                const float dvdx = ( v1x - v0x ) / 2.0f;

                const float w0x = src_values[ ( index + i0 ) * veclen + 2 ];
                const float w1x = src_values[ ( index + i1 ) * veclen + 2 ];
                const float dwdx = ( w1x - w0x ) / 2.0f;

                const kvs::Vector3f v( src_values + index * veclen );
                const kvs::Vector3f w( dwdy - dvdz, dudz - dwdx, dvdx - dudy );
                const float vw = v.length() * w.length();
                if ( vw != 0 ) *(pvalues++) = v.dot( w ) / vw;
                else *(pvalues++) = -1.0f;
            }
        }
    }

    SuperClass::setGridType( volume->gridType() );
    SuperClass::setResolution( volume->resolution() );
    SuperClass::setVeclen( 1 );
    SuperClass::setValues( kvs::AnyValueArray( values ) );
    SuperClass::updateMinMaxValues();
    SuperClass::updateMinMaxCoords();

    return this;
}

} // end of namespace util

} // end of namespace kvsoceanvis
