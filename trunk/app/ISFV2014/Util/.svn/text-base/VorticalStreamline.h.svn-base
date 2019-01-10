#pragma once

#include <kvs/Module>
#include <kvs/StreamlineBase>

#include <kvs/Type>
#include <kvs/Message>
#include <kvs/RGBColor>
#include <kvs/Vector3>
#include <kvs/VolumeObjectBase>

/*
namespace
{

template <typename T>
inline const kvs::Vec3 GetInterpolatedVector(
    const size_t vertex_id[8],
    const float weight[8],
    const kvs::VolumeObjectBase* volume )
{
    const T* values = reinterpret_cast<const T*>( volume->values().data() );

    kvs::Vec3 ret( 0.0f, 0.0f, 0.0f );
    for ( size_t i = 0; i < 8; i++ )
    {
        const size_t index = 3 * vertex_id[i];
        const float w = weight[i];
        ret.x() += static_cast<float>( values[ index     ] * w );
        ret.y() += static_cast<float>( values[ index + 1 ] * w );
        ret.z() += static_cast<float>( values[ index + 2 ] * w );
    }

    return ret;
}

int BinarySearch( const float v, const float* array, const size_t size )
{
    int low = 0;
    int middle = 0;
    int high = size - 1;

    while ( low <= high )
    {
        middle = ( low + high ) / 2;

        if ( array[ middle ] <= v )
        {
            if ( array[ middle + 1 ] > v ) { return middle; }
            else { low = middle + 1; }
        }
        else if ( array[ middle ] > v )
        {
            if ( array[ middle - 1 ] <= v ) { return middle - 1; }
            else { high = middle - 1; }
        }
        else { low = middle + 1; }
    }

    return -1;
}

kvs::Vec3i IndexOf( const kvs::Vec3& vertex, const kvs::StructuredVolumeObject* volume )
{
    const size_t dimx = volume->resolution().x();
    const size_t dimy = volume->resolution().y();
    const size_t dimz = volume->resolution().z();
    const float* xcoords = volume->coords().data();
    const float* ycoords = volume->coords().data() + dimx;
    const float* zcoords = volume->coords().data() + dimx + dimy;
    const int xindex = BinarySearch( vertex.x(), xcoords, dimx );
    const int yindex = BinarySearch( vertex.y(), ycoords, dimy );
    const int zindex = BinarySearch( vertex.z(), zcoords, dimz );
    return kvs::Vec3i( xindex, yindex, zindex);
}

} // end of namespace
*/


namespace ISFV2014
{

class VorticalStreamline : public kvs::StreamlineBase
{
    kvsModule( ISFV2014::VorticalStreamline, Mapper );
    kvsModuleBaseClass( kvs::StreamlineBase );

public:

    VorticalStreamline() {}

    virtual ~VorticalStreamline() {}

    BaseClass::SuperClass* exec( const kvs::ObjectBase* object )
    {
        if ( !object )
        {
            BaseClass::setSuccess( false );
            kvsMessageError("Input object is NULL.");
            return NULL;
        }

        const kvs::VolumeObjectBase* volume = kvs::VolumeObjectBase::DownCast( object );
        if ( !volume )
        {
            BaseClass::setSuccess( false );
            kvsMessageError("Input object is not volume dat.");
            return NULL;
        }

        // Check whether the volume can be processed or not.
        if ( volume->veclen() != 3 )
        {
            BaseClass::setSuccess( false );
            kvsMessageError("Input volume is not vector field data.");
            return NULL;
        }

        // Attach the pointer to the volume object.
        BaseClass::attachVolume( volume );
        BaseClass::setRange( volume );
        BaseClass::setMinMaxCoords( volume, this );

        // set the min/max vector length.
        if ( !volume->hasMinMaxValues() )
        {
            volume->updateMinMaxValues();
        }

        BaseClass::mapping( volume );

        return this;
    }

private:

    bool check_for_acceptance( const std::vector<kvs::Real32>& /* vertices */ )
    {
        return true;
    }

    bool check_for_termination(
        const kvs::Vec3& /* current_vertex */,
        const kvs::Vec3& direction,
        const size_t integration_times,
        const kvs::Vec3& next_vertex )
    {
        if ( m_enable_boundary_condition )
        {
            if ( !BaseClass::check_for_inside_volume( next_vertex ) ) return true;
        }

        if ( m_enable_vector_length_condition )
        {
            if ( BaseClass::check_for_vector_length( direction ) ) return true;
        }

        if ( m_enable_integration_times_condition )
        {
            if ( BaseClass::check_for_integration_times( integration_times ) ) return true;
        }

        return false;
    }

    const kvs::Vec3 calculate_vector( const kvs::Vec3& vertex )
    {
        const kvs::Vec3 origin( 0.0f, 0.0f, 0.0f );
        return this->interpolate_vector( vertex, origin );
    }

    const kvs::RGBColor calculate_color( const kvs::Vec3& direction )
    {
        const kvs::Real64 min_length = BaseClass::volume()->minValue();
        const kvs::Real64 max_length = BaseClass::volume()->maxValue();
        const kvs::Real64 diff = direction.length() - min_length;
        const kvs::Real64 interval = max_length - min_length;
        const kvs::UInt8 level = kvs::UInt8( 255.0 * diff / interval );

        return BaseClass::transferFunction().colorMap()[level];
    }

    const kvs::Vec3 interpolate_vector( const kvs::Vec3& vertex, const kvs::Vec3& /* direction */ )
    {
        const kvs::StructuredVolumeObject* volume = kvs::StructuredVolumeObject::DownCast( BaseClass::volume() );

        const size_t dimx = volume->resolution().x();
        const size_t dimy = volume->resolution().y();
        const kvs::Vec3i index = ::IndexOf( vertex, volume );

        size_t vertex_id[8];
        vertex_id[0] = index.x() + index.y() * dimx + index.z() * dimx * dimy;
        vertex_id[1] = vertex_id[0] + 1;
        vertex_id[2] = vertex_id[1] + dimx;
        vertex_id[3] = vertex_id[2] - 1;
        vertex_id[4] = vertex_id[0] + dimx * dimy;
        vertex_id[5] = vertex_id[4] + 1;
        vertex_id[6] = vertex_id[5] + dimx;
        vertex_id[7] = vertex_id[6] - 1;

        const float* xcoords = volume->coords().data();
        const float* ycoords = volume->coords().data() + dimx;
        const float* zcoords = volume->coords().data() + dimx + dimy;

        const float basex = xcoords[ index.x() ];
        const float basey = ycoords[ index.y() ];
        const float basez = zcoords[ index.z() ];
        const float diffx = xcoords[ index.x() + 1 ] - basex;
        const float diffy = ycoords[ index.y() + 1 ] - basey;
        const float diffz = zcoords[ index.z() + 1 ] - basez;
        const kvs::Vec3 local_coord(
            ( vertex.x() - basex ) / diffx,
            ( vertex.y() - basey ) / diffy,
            ( vertex.z() - basez ) / diffz );

        const float x = local_coord.x();
        const float y = local_coord.y();
        const float z = local_coord.z();
        float weight[8];
        weight[0] = (1-x)*(1-y)*   z ;
        weight[1] =    x *(1-y)*   z ;
        weight[2] =    x *   y *   z ;
        weight[3] = (1-x)*   y *   z ;
        weight[4] = (1-x)*(1-y)*(1-z);
        weight[5] =    x *(1-y)*(1-z);
        weight[6] =    x *   y *(1-z);
        weight[7] = (1-x)*   y *(1-z);

        // Interpolate.
        switch ( volume->values().typeID() )
        {
        case kvs::Type::TypeInt8: return ::GetInterpolatedVector<kvs::Int8>( vertex_id, weight, volume );
        case kvs::Type::TypeInt16: return ::GetInterpolatedVector<kvs::Int16>( vertex_id, weight, volume );
        case kvs::Type::TypeInt32: return ::GetInterpolatedVector<kvs::Int32>( vertex_id, weight, volume );
        case kvs::Type::TypeInt64: return ::GetInterpolatedVector<kvs::Int64>( vertex_id, weight, volume );
        case kvs::Type::TypeUInt8: return ::GetInterpolatedVector<kvs::UInt8>( vertex_id, weight, volume );
        case kvs::Type::TypeUInt16: return ::GetInterpolatedVector<kvs::UInt16>( vertex_id, weight, volume );
        case kvs::Type::TypeUInt32: return ::GetInterpolatedVector<kvs::UInt32>( vertex_id, weight, volume );
        case kvs::Type::TypeUInt64: return ::GetInterpolatedVector<kvs::UInt64>( vertex_id, weight, volume );
        case kvs::Type::TypeReal32: return ::GetInterpolatedVector<kvs::Real32>( vertex_id, weight, volume );
        case kvs::Type::TypeReal64: return ::GetInterpolatedVector<kvs::Real64>( vertex_id, weight, volume );
        default: break;
        }

        return kvs::Vec3::Zero();
    }
};

} // end of namespace ISFV2014
