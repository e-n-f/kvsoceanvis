/*****************************************************************************/
/**
 *  @file   Isosurface.h
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: Isosurface.h 287 2012-06-15 14:20:06Z naohisa.sakamoto $
 */
/*****************************************************************************/
#ifndef KVSOCEANVIS__UTIL__ISOSURFACE_H_INCLUDE
#define KVSOCEANVIS__UTIL__ISOSURFACE_H_INCLUDE

#include <kvs/PolygonObject>
#include <kvs/StructuredVolumeObject>
#include <kvs/MapperBase>
#include <kvs/ClassName>
#include <kvs/Module>


namespace kvsoceanvis
{

namespace util
{

class Isosurface : public kvs::MapperBase, public kvs::PolygonObject
{
    // Class name.
    kvsClassName( kvsoceanvis::util::Isosurface );

    // Module information.
    kvsModuleCategory( Mapper );
    kvsModuleBaseClass( kvs::MapperBase );
    kvsModuleSuperClass( kvs::PolygonObject );

private:

    double m_isolevel;    ///< isosurface level
    bool   m_duplication; ///< duplication flag

public:

    Isosurface(
        const kvs::StructuredVolumeObject* volume,
        const double                       isolevel,
        const SuperClass::NormalType       normal_type,
        const bool                         duplication,
        const kvs::TransferFunction&       transfer_function );

public:

    SuperClass* exec( const kvs::ObjectBase* object );

private:

    const size_t calculate_table_index( const size_t* local_index ) const;
    const kvs::Vector3f interpolate_vertex(
        const kvs::Vector3f& index0,
        const kvs::Vector3f& index1,
        const kvs::Vector3f& vertex0,
        const kvs::Vector3f& vertex1 ) const;
    const kvs::RGBColor calculate_color( void );
};

} // end of namepsace util

} // end of namepsace kvsoceanvis

#endif // KVSOCEANVIS__UTIL__ISOSURFACE_H_INCLUDE
