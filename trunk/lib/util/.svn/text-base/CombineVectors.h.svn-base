/*****************************************************************************/
/**
 *  @file   CombineVectors.h
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
#ifndef KVSOCEANVIS__UTIL__COMBINE_VECTORS_H_INCLUDE
#define KVSOCEANVIS__UTIL__COMBINE_VECTORS_H_INCLUDE

#include <kvs/StructuredVolumeObject>
#include <kvs/FilterBase>
#include <kvs/ClassName>
#include <kvs/Module>


namespace kvsoceanvis
{

namespace util
{

/*===========================================================================*/
/**
 *  @brief  Combine vectors class.
 */
/*===========================================================================*/
class CombineVectors : public kvs::FilterBase, public kvs::StructuredVolumeObject
{
    // Class name.
    kvsClassName( kvsoceanvis::util::CombineVectors );

    // Module information.
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::StructuredVolumeObject );

private:

    const kvs::StructuredVolumeObject* m_volume2; ///< second component value
    const kvs::StructuredVolumeObject* m_volume3; ///< third component value

public:

    CombineVectors(
        const kvs::StructuredVolumeObject* uvolume,
        const kvs::StructuredVolumeObject* vvolume,
        const kvs::StructuredVolumeObject* wvolume );

    SuperClass* exec( const kvs::ObjectBase* object );
};

} // end of namespace util

} // end of namespcae kvsoceanvis

#endif // KVSOCEANVIS__UTIL__COMBINE_VECTORS_H_INCLUDE
