/*****************************************************************************/
/**
 *  @file   AdjustVectorScale.h
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
#ifndef KVSOCEANVIS__UTIL__ADJUST_VECTOR_SCALE_H_INCLUDE
#define KVSOCEANVIS__UTIL__ADJUST_VECTOR_SCALE_H_INCLUDE

#include <kvs/StructuredVolumeObject>
#include <kvs/FilterBase>
#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/ValueArray>


namespace kvsoceanvis
{

namespace util
{

class AdjustVectorScale : public kvs::FilterBase, public kvs::StructuredVolumeObject
{
    // Class name.
    kvsClassName( kvsoceanvis::util::AdjustVectorScale );

    // Module information.
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::StructuredVolumeObject );

private:

    kvs::Real32 m_earth_radius;
    kvs::Real32 m_depth_scale;

public:

    static const kvs::Real32 EarthRadius;

public:

    AdjustVectorScale(
        const kvs::StructuredVolumeObject* volume,
        const kvs::Real32 earth_radius,
        const kvs::Real32 depth_scale );

    SuperClass* exec( const kvs::ObjectBase* object );
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__ADJUST_VECTOR_SCALE_H_INCLUDE
