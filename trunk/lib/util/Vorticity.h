/*****************************************************************************/
/**
 *  @file   Vorticity.h
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
#ifndef KVSOCEANVIS__UTIL__VORTICITY_H_INCLUDE
#define KVSOCEANVIS__UTIL__VORTICITY_H_INCLUDE

#include <kvs/StructuredVolumeObject>
#include <kvs/FilterBase>
#include <kvs/ClassName>
#include <kvs/Module>


namespace kvsoceanvis
{

namespace util
{

class Vorticity : public kvs::FilterBase, public kvs::StructuredVolumeObject
{
    // Class name.
    kvsClassName( kvsoceanvis::util::Vorticity );

    // Module information.
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::StructuredVolumeObject );

public:

    Vorticity( const kvs::StructuredVolumeObject* volume );

    SuperClass* exec( const kvs::ObjectBase* object );
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__VORTICITY_H_INCLUDE
