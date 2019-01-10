/*****************************************************************************/
/**
 *  @file   OrthoSlice.cpp
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: OrthoSlice.cpp 287 2012-06-15 14:20:06Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include "OrthoSlice.h"


namespace
{

const kvs::Matrix33f Normal(
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f );

}


namespace kvsoceanvis
{

namespace util
{

OrthoSlice::OrthoSlice(
    const kvs::VolumeObjectBase* volume,
    const float position,
    const OrthoSlice::AlignedAxis axis,
    const kvs::TransferFunction& transfer_function ):
    util::SlicePlane( volume, ::Normal[axis] * position, ::Normal[axis], transfer_function )
{
}

void OrthoSlice::setPlane( const float position, const util::OrthoSlice::AlignedAxis axis )
{
    SuperClass::setPlane( ::Normal[axis] * position, ::Normal[axis] );
}

} // end of namespace util

} // end of namespace kvsoceanvis
