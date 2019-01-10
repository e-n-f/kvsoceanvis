#pragma once

#include <kvs/Math>
#include <kvs/GrADS>
#include <kvs/StructuredVolumeObject>
#include <util/StructuredVolumeImporter.h>
#include <util/CombineVectors.h>

using namespace kvsoceanvis;


namespace
{

inline float Interp( float s0, float s1, float d0, float d1, float d )
{
    //  s0 --- s ----- s1
    // (d0)   (d)     (d1)
    const float a = ( d - d0 ) / ( d1 - d0 );
    return kvs::Math::Mix( s0, s1, a );
}

inline float Extrap( float s0, float s1, float d0, float d1, float d )
{
    if ( d < d0 )
    {
        //  s --- s0 ----- s1
        // (d)   (d0)     (d1)
        const float a = ( d0 - d ) / ( d1 - d );
        const float A = 1.0f - 1.0f / ( 1.0f - a );
        return kvs::Math::Mix( s0, s1, A );
    }
    else if ( d1 < d )
    {
        //  s0 --- s1 ----- s
        // (d0)   (d1)     (d)
        const float a = ( d1 - d0 ) / ( d - d0 );
        const float A = 1.0f / a;
        return kvs::Math::Mix( s0, s1, A );
    }

    return Interp( s0, s1, d0, d1, d );
}

inline kvs::ValueArray<kvs::Real32> AdjustByDepth(
    const kvs::StructuredVolumeObject* uvolume,
    const kvs::StructuredVolumeObject* wvolume )
{
    const size_t dimx = uvolume->resolution().x();
    const size_t dimy = uvolume->resolution().y();
    const size_t dimz = uvolume->resolution().z();
    const kvs::Real32* udepth = uvolume->coords().data() + dimx + dimy;
    const kvs::Real32* wdepth = wvolume->coords().data() + dimx + dimy;
    const kvs::Real32* wvalues = wvolume->values().asValueArray<kvs::Real32>().data();

    kvs::ValueArray<kvs::Real32> values( wvolume->values().size() );

    //  udepth: x----x------x--------x------------x
    //  wdepth:  *----*----*------*---------*----------*
    for ( size_t k = 0, K = 0; k < dimz; k++ )
    {
        const float d = udepth[k];
        if ( d < wdepth[0] )
        {
            const float s0 = wvalues[0];
            const float s1 = wvalues[1];
            const float d0 = wdepth[0];
            const float d1 = wdepth[1];
            values[k] = Extrap( s0, s1, d0, d1, d );
        }
        else if ( d > wdepth[dimz-1] )
        {
            const float s0 = wvalues[dimz-2];
            const float s1 = wvalues[dimz-1];
            const float d0 = wdepth[dimz-2];
            const float d1 = wdepth[dimz-1];
            values[k] = Extrap( s0, s1, d0, d1, d );
        }
        else
        {
            for ( size_t i = K; i < dimz - 1; i++ )
            {
                const float d0 = wdepth[i];
                const float d1 = wdepth[i+1];
                if ( d0 <= d && d < d1 )
                {
                    const float s0 = wvalues[i];
                    const float s1 = wvalues[i+1];
                    values[k] = Interp( s0, s1, d0, d1, d );
                    break;
                }
                else { k++; }
            }
        }
    }

    return values;
}

}

namespace ISFV2014
{

kvs::StructuredVolumeObject* Import(
    const kvs::GrADS* data,
    const size_t tindex,
    const size_t vindex,
    const bool zflip = true )
{
    typedef kvs::StructuredVolumeObject Object;
    typedef util::StructuredVolumeImporter Importer;

    const Object::GridType grid_type = Object::Rectilinear;
    Object* object = new Importer( data, vindex, tindex, zflip, grid_type );

    return object;
}

kvs::StructuredVolumeObject* Import(
    const kvs::GrADS* udata,
    const kvs::GrADS* vdata,
    const kvs::GrADS* wdata,
    const size_t tindex,
    const size_t vindex,
    const bool zflip = true,
    const bool wadjust = true )
{
    typedef kvs::StructuredVolumeObject Object;
    typedef util::StructuredVolumeImporter Importer;

    const Object::GridType grid_type = Object::Rectilinear;
    Object* uvolume = new Importer( udata, vindex, tindex, zflip, grid_type );
    Object* vvolume = new Importer( vdata, vindex, tindex, zflip, grid_type );
    Object* wvolume = new Importer( wdata, vindex, tindex, zflip, grid_type );

    if ( wadjust )
    {
        wvolume->setCoords( uvolume->coords() );
        wvolume->setValues( kvs::AnyValueArray( ::AdjustByDepth( uvolume, wvolume ) ) );
    }

    kvs::StructuredVolumeObject* object = new util::CombineVectors( uvolume, vvolume, wvolume );

    delete uvolume;
    delete vvolume;
    delete wvolume;

    return object;
}

} // end of namespace ISFV2014
