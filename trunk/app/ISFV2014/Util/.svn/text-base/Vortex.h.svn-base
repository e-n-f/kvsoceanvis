#pragma once

#include <kvs/PointObject>
#include <kvs/StructuredVolumeObject>
#include <util/VortexPointVolume.h>
#include <util/VortexPoint.h>


namespace ISFV2014
{

kvs::PointObject* Vortex(
    const kvs::StructuredVolumeObject* volume,
    const float ignore_value )
{
    kvs::UnstructuredVolumeObject* tensor = new util::VortexPointVolume( volume, ignore_value );
    kvs::PointObject* point = new util::VortexPoint( tensor );
    delete tensor;

    return point;
}

} // end of namespace ISFV2014
