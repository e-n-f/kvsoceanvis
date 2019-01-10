/*****************************************************************************/
/**
 *  @file   OrthoSlice.h
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
#ifndef KVSOCEANVIS__UTIL__ORTHO_SLICE_H_INCLUDE
#define KVSOCEANVIS__UTIL__ORTHO_SLICE_H_INCLUDE

#include "SlicePlane.h"
#include <kvs/VolumeObjectBase>
#include <kvs/ClassName>
#include <kvs/Module>


namespace kvsoceanvis
{

namespace util
{

class OrthoSlice : public util::SlicePlane
{
    // Class name.
    kvsClassName( kvsoceanvis::util::OrthoSlice );

    // Module information.
    kvsModuleCategory( Mapper );
    kvsModuleSuperClass( util::SlicePlane );

public:

    enum AlignedAxis
    {
        XAxis = 0,
        YAxis = 1,
        ZAxis = 2
    };

protected:

    AlignedAxis m_aligned_axis; ///< aligned axis

public:

    OrthoSlice(
        const kvs::VolumeObjectBase* volume,
        const float position,
        const AlignedAxis axis,
        const kvs::TransferFunction& transfer_function );

public:

    void setPlane( const float position, const util::OrthoSlice::AlignedAxis axis );
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__ORTHO_SLICE_H_INCLUDE
