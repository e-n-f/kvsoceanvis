#pragma once

#include <kvs/Vector3>
#include <kvs/StructuredVolumeObject>
#include <util/CropVolume.h>
#include <util/DegreeToDistance.h>

using namespace kvsoceanvis;

namespace ISFV2014
{

kvs::StructuredVolumeObject* Crop(
    const kvs::StructuredVolumeObject* volume,
    const kvs::Vec3 min_range,
    const kvs::Vec3 max_range )
{
    kvs::StructuredVolumeObject* cropped = new util::CropVolume( volume, min_range, max_range );
    kvs::StructuredVolumeObject* object = new util::DegreeToDistance( cropped ); // deg to kilo-meter
    delete cropped;

    return object;
};

} // end of namespace ISFV2014
