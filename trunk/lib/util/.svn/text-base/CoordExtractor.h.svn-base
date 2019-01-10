/*****************************************************************************/
/**
 *  @file   CoordExtractor.h
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
#ifndef KVSOCEANVIS__UTIL__COORD_EXTRACTOR_H_INCLUDE
#define KVSOCEANVIS__UTIL__COORD_EXTRACTOR_H_INCLUDE

#include <kvs/GrADS>
#include <kvs/ValueArray>
#include <kvs/Vector3>


namespace kvsoceanvis
{

namespace util
{

class CoordExtractor
{
protected:

    const kvs::GrADS* m_grads; ///< pointer to GrADS data
    kvs::Vector3f m_min_range; ///< minimum range
    kvs::Vector3f m_max_range; ///< maximum range

public:

    CoordExtractor( const kvs::GrADS* grads );
    CoordExtractor( const kvs::GrADS* grads, const kvs::Vector3f& min_range, const kvs::Vector3f& max_range );

    void setRange( const kvs::Vector3f& min_range, const kvs::Vector3f& max_range );

    kvs::ValueArray<kvs::Real32> alignedCoordX( void ) const;
    kvs::ValueArray<kvs::Real32> alignedCoordY( void ) const;
    kvs::ValueArray<kvs::Real32> alignedCoordZ( void ) const;

private:

    size_t dimx( void ) const;
    size_t dimy( void ) const;
    size_t dimz( void ) const;

    kvs::ValueArray<kvs::Real32> cropped_coord_x( void ) const;
    kvs::ValueArray<kvs::Real32> cropped_coord_y( void ) const;
    kvs::ValueArray<kvs::Real32> cropped_coord_z( void ) const;
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__COORD_EXTRACTOR_H_INCLUDE
