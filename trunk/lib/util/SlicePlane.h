/*****************************************************************************/
/**
 *  @file   SlicePlane.h
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
#ifndef KVSOCEANVIS__UTIL__SLICE_PLANE_H_INCLUDE
#define KVSOCEANVIS__UTIL__SLICE_PLANE_H_INCLUDE

#include <kvs/Vector3>
#include <kvs/Vector4>
#include <kvs/MapperBase>
#include <kvs/ClassName>
#include <kvs/Module>
#include <kvs/PolygonObject>
#include <kvs/StructuredVolumeObject>


namespace kvsoceanvis
{

namespace util
{

class SlicePlane : public kvs::MapperBase, public kvs::PolygonObject
{
    // Class name.
    kvsClassName( kvsoceanvis::util::SlicePlane );

    // Module information.
    kvsModuleCategory( Mapper );
    kvsModuleBaseClass( kvs::MapperBase );
    kvsModuleSuperClass( kvs::PolygonObject );

private:

    kvs::Vector4f m_coefficients; ///< coeficients of a slice plane

public:

    SlicePlane(
        const kvs::VolumeObjectBase* volume,
        const kvs::Vector4f&         coefficients,
        const kvs::TransferFunction& transfer_function );

    SlicePlane(
        const kvs::VolumeObjectBase* volume,
        const kvs::Vector3f&         point,
        const kvs::Vector3f&         normal,
        const kvs::TransferFunction& transfer_function );

public:

    void setPlane( const kvs::Vector4f& coefficients );
    void setPlane( const kvs::Vector3f& point, const kvs::Vector3f& normal );

public:

    SuperClass* exec( const kvs::ObjectBase* object );

private:

    const size_t calculate_table_index(
        const size_t x,
        const size_t y,
        const size_t z ) const;

    const size_t calculate_table_index(
        const float x,
        const float y,
        const float z,
        const float xdiff,
        const float ydiff,
        const float zdiff ) const;

    const float substitute_plane_equation(
        const size_t x,
        const size_t y,
        const size_t z ) const;

    const float substitute_plane_equation(
        const kvs::Vector3f& vertex ) const;

    const kvs::Vector3f interpolate_vertex(
        const kvs::Vector3f& vertex0,
        const kvs::Vector3f& vertex1 ) const;

    const double interpolate_value(
        const kvs::StructuredVolumeObject* volume,
        const kvs::Vector3f& index0,
        const kvs::Vector3f& index1,
        const kvs::Vector3f& vertex0,
        const kvs::Vector3f& vertex1 ) const;
};

} // end of namespace util

} // end of namespace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__SLICE_PLANE_H_INCLUDE
