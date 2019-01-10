/*****************************************************************************/
/**
 *  @file   CoordExtractor.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: CoordExtractor.cpp 287 2012-06-15 14:20:06Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "CoordExtractor.h"


namespace kvsoceanvis
{

namespace util
{

CoordExtractor::CoordExtractor( const kvs::GrADS* grads )
{
    m_grads = grads;

    const size_t dimx = m_grads->dataDescriptor().xdef().num;
    const size_t dimy = m_grads->dataDescriptor().ydef().num;
    const size_t dimz = m_grads->dataDescriptor().zdef().num;
    this->setRange( kvs::Vector3f( 0, 0, 0 ), kvs::Vector3f( dimx-1, dimy-1, dimz-1 ) );
}

CoordExtractor::CoordExtractor( const kvs::GrADS* grads, const kvs::Vector3f& min_range, const kvs::Vector3f& max_range )
{
    m_grads = grads;
    this->setRange( min_range, max_range );
}

void CoordExtractor::setRange( const kvs::Vector3f& min_range, const kvs::Vector3f& max_range )
{
    m_min_range = min_range;
    m_max_range = max_range;
}

kvs::ValueArray<kvs::Real32> CoordExtractor::alignedCoordX( void ) const
{
    const kvs::ValueArray<kvs::Real32> coord = this->cropped_coord_x();
    const size_t dimx = this->dimx();
    const size_t dimy = this->dimy();
    const size_t dimz = this->dimz();
    const size_t size = dimx * dimy * dimz;
    kvs::ValueArray<kvs::Real32> ret( size );
    for ( size_t k = 0, index = 0; k < dimz; ++k )
    {
        for ( size_t j = 0; j < dimy; ++j )
        {
            for ( size_t i = 0; i < dimx; ++i, ++index )
            {
                ret[index] = coord[i];
            }
        }
    }

    return ret;
}

kvs::ValueArray<kvs::Real32> CoordExtractor::alignedCoordY( void ) const
{
    const kvs::ValueArray<kvs::Real32> coord = this->cropped_coord_y();
    const size_t dimx = this->dimx();
    const size_t dimy = this->dimy();
    const size_t dimz = this->dimz();
    const size_t size = dimx * dimy * dimz;
    kvs::ValueArray<kvs::Real32> ret( size );
    for ( size_t k = 0, index = 0; k < dimz; ++k )
    {
        for ( size_t j = 0; j < dimy; ++j )
        {
            for ( size_t i = 0; i < dimx; ++i, ++index )
            {
                ret[index] = coord[j];
            }
        }
    }

    return ret;
}

kvs::ValueArray<kvs::Real32> CoordExtractor::alignedCoordZ( void ) const
{
    const kvs::ValueArray<kvs::Real32> coord = this->cropped_coord_z();
    const size_t dimx = this->dimx();
    const size_t dimy = this->dimy();
    const size_t dimz = this->dimz();
    const size_t size = dimx * dimy * dimz;
    kvs::ValueArray<kvs::Real32> ret( size );
    for ( size_t k = 0, index = 0; k < dimz; ++k )
    {
        for ( size_t j = 0; j < dimy; ++j )
        {
            for ( size_t i = 0; i < dimx; ++i, ++index )
            {
                ret[index] = coord[k];
            }
        }
    }

    return ret;
}

size_t CoordExtractor::dimx( void ) const
{
    return m_max_range.x() - m_min_range.x() + 1;
}

size_t CoordExtractor::dimy( void ) const
{
    return m_max_range.y() - m_min_range.y() + 1;
}

size_t CoordExtractor::dimz( void ) const
{
    return m_max_range.z() - m_min_range.z() + 1;
}

kvs::ValueArray<kvs::Real32> CoordExtractor::cropped_coord_x( void ) const
{
    const size_t dim = this->dimx();
    kvs::ValueArray<kvs::Real32> ret( dim );
    if ( m_grads->dataDescriptor().xdef().mapping == kvs::grads::XDef::Levels )
    {
        const kvs::ValueArray<kvs::Real32> values = m_grads->dataDescriptor().xdef().values;
        for ( size_t i = 0, j = m_min_range.x(); i < dim; i++, j++ ) ret[i] = values[j];
    }
    else if ( m_grads->dataDescriptor().xdef().mapping == kvs::grads::XDef::Linear )
    {
        const kvs::Real32 increment = m_grads->dataDescriptor().xdef().values[1];
        kvs::Real32 value = m_grads->dataDescriptor().xdef().values[0];
        value += increment * m_min_range.x();
        for ( size_t i = 0; i < dim; i++, value += increment ) ret[i] = value;
    }

    return ret;
}

kvs::ValueArray<kvs::Real32> CoordExtractor::cropped_coord_y( void ) const
{
    const size_t dim = this->dimy();
    kvs::ValueArray<kvs::Real32> ret( dim );
    if ( m_grads->dataDescriptor().ydef().mapping == kvs::grads::YDef::Levels )
    {
        const kvs::ValueArray<kvs::Real32> values = m_grads->dataDescriptor().ydef().values;
        for ( size_t i = 0, j = m_min_range.y(); i < dim; i++, j++ ) ret[i] = values[j];
    }
    else if ( m_grads->dataDescriptor().ydef().mapping == kvs::grads::YDef::Linear )
    {
        const kvs::Real32 increment = m_grads->dataDescriptor().ydef().values[1];
        kvs::Real32 value = m_grads->dataDescriptor().ydef().values[0];
        value += increment * m_min_range.y();
        for ( size_t i = 0; i < dim; i++, value += increment ) ret[i] = value;
    }

    return ret;
}

kvs::ValueArray<kvs::Real32> CoordExtractor::cropped_coord_z( void ) const
{
    const size_t dim = this->dimz();
    kvs::ValueArray<kvs::Real32> ret( dim );
    if ( m_grads->dataDescriptor().zdef().mapping == kvs::grads::ZDef::Levels )
    {
        const kvs::ValueArray<kvs::Real32> values = m_grads->dataDescriptor().zdef().values;
        for ( size_t i = 0, j = m_min_range.z(); i < dim; i++, j++ ) ret[i] = values[j];
    }
    else if ( m_grads->dataDescriptor().zdef().mapping == kvs::grads::ZDef::Linear )
    {
        const kvs::Real32 increment = m_grads->dataDescriptor().zdef().values[1];
        kvs::Real32 value = m_grads->dataDescriptor().zdef().values[0];
        value += increment * m_min_range.z();
        for ( size_t i = 0; i < dim; i++, value += increment ) ret[i] = value;
    }

    return ret;
}

} // end of namespace util

} // end of namespace kvsoceanvis
