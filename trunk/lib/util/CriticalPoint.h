/*****************************************************************************/
/**
 *  @file   CriticalPoint.h
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
#ifndef KVSOCEANVIS__UTIL__CRITICAL_POINT_H_INCLUDE
#define KVSOCEANVIS__UTIL__CRITICAL_POINT_H_INCLUDE

#include <kvs/UnstructuredVolumeObject>
#include <kvs/PointObject>
#include <kvs/MapperBase>
#include <kvs/ClassName>
#include <kvs/Module>


namespace kvsoceanvis
{

namespace util
{

class CriticalPoint : public kvs::MapperBase, public kvs::PointObject
{
    // Class name.
    kvsClassName( kvsoceanvis::util::CriticalPoint );

    // Module information.
    kvsModuleCategory( Mapper );
    kvsModuleBaseClass( kvs::MapperBase );
    kvsModuleSuperClass( kvs::PointObject );

public:

    CriticalPoint( const kvs::UnstructuredVolumeObject* volume );

    SuperClass* exec( const kvs::ObjectBase* object );
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__CRITICAL_POINT_H_INCLUDE
