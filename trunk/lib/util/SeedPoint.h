/*****************************************************************************/
/**
 *  @file   SeedPoint.h
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
#ifndef KVSOCEANVIS__UTIL__SEED_POINT_H_INCLUDE
#define KVSOCEANVIS__UTIL__SEED_POINT_H_INCLUDE

#include <kvs/PointObject>
#include <kvs/FilterBase>
#include <kvs/ClassName>
#include <kvs/Module>


namespace kvsoceanvis
{

namespace util
{

class SeedPoint : public kvs::FilterBase, public kvs::PointObject
{
    // Class name.
    kvsClassName( kvsoceanvis::util::SeedPoint );

    // Module information.
    kvsModuleCategory( Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::PointObject );

private:

    float m_delta;

public:

    SeedPoint( const kvs::PointObject* point, const float delta = 0.1f );

    SuperClass* exec( const kvs::ObjectBase* object );
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__SEED_POINT_H_INCLUDE
