/*****************************************************************************/
/**
 *  @file   DegreeToDistance.h
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
#ifndef KVSOCEANVIS__UTIL__DEGREE_TO_DISTANCE_H_INCLUDE
#define KVSOCEANVIS__UTIL__DEGREE_TO_DISTANCE_H_INCLUDE

#include <kvs/StructuredVolumeObject>
#include <kvs/FilterBase>
#include <kvs/ClassName>
#include <kvs/Module>


namespace kvsoceanvis
{

namespace util
{

class DegreeToDistance : public kvs::FilterBase, public kvs::StructuredVolumeObject
{
    kvsModule( kvsoceanvis::util::DegreeToDistance, Filter );
    kvsModuleBaseClass( kvs::FilterBase );
    kvsModuleSuperClass( kvs::StructuredVolumeObject );

private:

    float m_longitude_scale; ///< longitude scale for scaling degree to kilo-meter
    float m_latitude_scale; ///< latitude scale for scaling degree to kilo-meter
    float m_depth_scale; ///< depth scale for scaling meter to kilo-meter

public:

    DegreeToDistance(
        const kvs::StructuredVolumeObject* volume,
        const float longitude_scale = 91.0f,
        const float latitude_scale = 111.0f,
        const float depth_scale = 0.001f );

    SuperClass* exec( const kvs::ObjectBase* object );
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__DEGREE_TO_DISTANCE_H_INCLUDE
