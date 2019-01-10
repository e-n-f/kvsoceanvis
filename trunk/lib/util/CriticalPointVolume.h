/*****************************************************************************/
/**
 *  @file   CriticalPointVolume.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: CriticalPointVolume.h 287 2012-06-15 14:20:06Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__UTIL__CRITICAL_POINT_VOLUME_H_INCLUDE
#define KVSOCEANVIS__UTIL__CRITICAL_POINT_VOLUME_H_INCLUDE

#include <kvs/StructuredVolumeObject>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/PointObject>
#include <kvs/FilterBase>
#include <kvs/ClassName>
#include <kvs/Module>


namespace kvsoceanvis
{

namespace util
{

class CriticalPointVolume : public kvs::FilterBase, public kvs::UnstructuredVolumeObject
{
    // Class name.
    kvsClassName( kvsoceanvis::util::CriticalPointVolume );

    // Module information.
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::UnstructuredVolumeObject );

private:

    bool m_has_ignore_value;
    float m_ignore_value;

public:

    CriticalPointVolume( const kvs::StructuredVolumeObject* volume );
    CriticalPointVolume( const kvs::StructuredVolumeObject* volume, const float ignore_value );

    void setIgnoreValue( const float ignore_value );

    SuperClass* exec( const kvs::ObjectBase* object );

private:

    bool include_ignore_value( const float* values ) const;
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__CRITICAL_POINT_VOLUME_H_INCLUDE
