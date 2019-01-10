/*****************************************************************************/
/**
 *  @file   CropVolume.h
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
#ifndef KVSOCEANVIS__UTIL__CROP_VOLUME_H_INCLUDE
#define KVSOCEANVIS__UTIL__CROP_VOLUME_H_INCLUDE

#include <kvs/StructuredVolumeObject>
#include <kvs/FilterBase>
#include <kvs/Module>


namespace kvsoceanvis
{

namespace util
{

class CropVolume : public kvs::FilterBase, public kvs::StructuredVolumeObject
{
    kvsModuleName( kvsoceanvis::util::CropVolume );
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::StructuredVolumeObject );

private:

    kvs::Vector3f m_min_range;
    kvs::Vector3f m_max_range;

public:

    CropVolume(
        const kvs::StructuredVolumeObject* volume,
        const kvs::Vector3f min_range,
        const kvs::Vector3f max_range );

    SuperClass* exec( const kvs::ObjectBase* object );
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__CROP_VOLUME_H_INCLUDE
