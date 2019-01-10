/*****************************************************************************/
/**
 *  @file   CreateColorMap.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: CreateColorMap.h 287 2012-06-15 14:20:06Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__UTIL__CREATE_COLOR_MAP_H_INCLUDE
#define KVSOCEANVIS__UTIL__CREATE_COLOR_MAP_H_INCLUDE

#include <kvs/ColorMap>


namespace kvsoceanvis
{

namespace util
{

inline kvs::ColorMap CreateColorMap( void )
{
    kvs::ColorMap cmap( 256 );
    cmap.addPoint(   0, kvs::RGBColor( 255, 0,   0 ) );
    cmap.addPoint( 128, kvs::RGBColor(   0, 0,   0 ) );
    cmap.addPoint( 255, kvs::RGBColor(   0, 0, 255 ) );
    cmap.create();

    return( cmap );
}

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__CREATE_COLOR_MAP_H_INCLUDE
