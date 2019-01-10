#pragma once

#include <kvs/Vector3>
#include <kvs/StructuredVolumeObject>


namespace ISFV2014
{

kvs::StructuredVolumeObject* Downsize(
    const kvs::StructuredVolumeObject* volume,
    const kvs::Vec3ui stride )
{
    const size_t dimx = ( volume->resolution().x() - 1 ) / stride.x() + 1;
    const size_t dimy = ( volume->resolution().y() - 1 ) / stride.y() + 1;
    const size_t dimz = ( volume->resolution().z() - 1 ) / stride.z() + 1;
    const size_t veclen = volume->veclen();

    const kvs::ValueArray<kvs::Real32>& volume_values = volume->values().asValueArray<kvs::Real32>();
    const kvs::ValueArray<kvs::Real32>& volume_coords = volume->coords();
    const kvs::Real32* volume_xcoords = volume_coords.data();
    const kvs::Real32* volume_ycoords = volume_xcoords + volume->resolution().x();
    const kvs::Real32* volume_zcoords = volume_ycoords + volume->resolution().y();

    kvs::ValueArray<kvs::Real32> coords( dimx + dimy + dimz );
    kvs::Real32* xcoords = coords.data();
    kvs::Real32* ycoords = xcoords + dimx;
    kvs::Real32* zcoords = ycoords + dimy;

    for ( size_t i = 0; i < volume->resolution().x(); i += stride.x() )
    {
        *(xcoords++) = volume_xcoords[i];
    }

    for ( size_t j = 0; j < volume->resolution().y(); j += stride.y() )
    {
        *(ycoords++) = volume_ycoords[j];
    }

    for ( size_t k = 0; k < volume->resolution().z(); k += stride.z() )
    {
        *(zcoords++) = volume_zcoords[k];
    }

    kvs::ValueArray<kvs::Real32> values( dimx * dimy * dimz * veclen );
    kvs::Real32* pvalues = values.data();
    for ( size_t k = 0; k < volume->resolution().z(); k += stride.z() )
    {
        const size_t kindex = k * volume->numberOfNodesPerSlice();
        for ( size_t j = 0; j < volume->resolution().y(); j += stride.y() )
        {
            const size_t jindex = j * volume->numberOfNodesPerLine();
            for ( size_t i = 0; i < volume->resolution().x(); i += stride.x() )
            {
                const size_t index = i + jindex + kindex;
                for ( size_t v = 0; v < veclen; v++ )
                {
                    *(pvalues++) = volume_values[ veclen * index + v ];
                }
            }
        }
    }

    kvs::StructuredVolumeObject* object = new kvs::StructuredVolumeObject();
    object->setGridTypeToRectilinear();
    object->setVeclen( veclen );
    object->setResolution( kvs::Vec3ui( dimx, dimy, dimz ) );
    object->setValues( kvs::AnyValueArray( values ) );
    object->setCoords( coords );
    object->setMinMaxValues( volume->minValue(), volume->maxValue() );
    object->updateMinMaxCoords();
    object->setMinMaxExternalCoords( volume->minExternalCoord(), volume->maxExternalCoord() );

    return object;
};

} // end of namespace ISFV2014
